/******************************* INCLUDES ************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <err.h>
#include "tools/vectors.h"
#include "shapes/material.h"
#include "tools/color.h"
#include "ray/ray.h"
#include "shapes/3Dshapes/sphere.h"
#include "shapes/3Dshapes/cube.h"
#include "shapes/3Dshapes/cylinder.h"
#include "shapes/2Dshapes/plane.h"
#include "shapes/3Dshapes/pyramid.h"
#include "shapes/3Dshapes/torus.h"
#include "shapes/3Dshapes/csg.h"
#include "shapes/shapes.h"

/*****************************************************************************/

struct shape_list *new_shape_list(void *obj, struct material *m,
        double (*inter)(void*, t_ray, int),
        t_vector (*normal)(t_vector, void*, int), void (*free)(void*))
{
    struct shape_list *sl = malloc(sizeof(struct shape_list));
    sl->obj = obj;
    sl->next = NULL;
    sl->m = new_material(m->diffuse->r, m->diffuse->g, m->diffuse->b,
            m->bump_mapped, m->textured);
    sl->intersect_shape = inter;
    sl->get_normal_shape = normal;
    sl->free_shape = free;
    return sl;
}

struct shape_list *add_shape(struct shape_list *sl, void *obj, struct material *m,
        double (*inter)(void*, t_ray, int), t_vector (*normal)(t_vector, void*, int),
        void (*free)(void*))
{
    struct shape_list *new_sl = new_shape_list(obj, m, inter, normal, free);
    struct shape_list *tmp = sl;

    if (sl == NULL)
        return new_sl;
    else
    {
        while (tmp->next)
            tmp = tmp->next;

        tmp->next = new_sl;
        return sl;
    }
}

struct shape_list *read_shapes(FILE *f, unsigned *line, char s[10])
{
    char shape[10];
    struct shape_list *sl = NULL;
    struct material *m = new_material(0, 0, 0, 0, 0);

    while (1)
    {
        if (fscanf(f, "%s\n", shape) != 1)
            break;

        ++(*line);

        if (strcmp(shape, "SPHERE") == 0)
            sl = add_shape(sl, read_sphere(f, line, m), m, intersect_sphere,
                    get_normal_sphere, free_sphere);
        else if (strcmp(shape, "PLANE") == 0)
            sl = add_shape(sl, read_plane(f, line, m), m, intersect_plane,
                    get_normal_plane, free_plane);
        else if (strcmp(shape, "CUBE") == 0)
            sl = add_shape(sl, read_cube(f, line, m), m, intersect_cube,
                    get_normal_cube, free_cube);
        //else if (strcmp(shape, "CYLINDER") == 0)
            //sl = add_shape(sl, 3, read_cylinder(f, line));
        else if (strcmp(shape, "PYRAMID") == 0)
            sl = add_shape(sl, read_pyramid(f, line, m), m, intersect_pyramid,
                    get_normal_pyramid, free_pyramid);
        else if (strcmp(shape, "TORUS") == 0)
            sl = add_shape(sl, read_torus(f, line, m), m, intersect_torus,
                    get_normal_torus, free_torus);
        else if (strcmp(shape, "CSGSHAPE") == 0)
            sl = add_shape(sl, read_csg_shape(f, line, m), m, intersect_csg,
                    get_normal_csg, free_csg);
        else
            break;
    }

    for (int i = 0; i < 10; ++i)
        s[i] = shape[i];

    free_material(m);

    return sl;
}

void free_shape_list(struct shape_list *sl)
{
    struct shape_list *tmp_sl = sl, *to_free;

    while (tmp_sl)
    {
        tmp_sl->free_shape(tmp_sl->obj);
        free(tmp_sl->m);
        to_free = tmp_sl;
        tmp_sl = tmp_sl->next;
        free(to_free);
    }
}

/*****************************************************************************/
