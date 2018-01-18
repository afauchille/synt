/******************************* INCLUDES ************************************/
#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <err.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "tools/vectors.h"
#include "shapes/material.h"
#include "tools/color.h"
#include "ray/ray.h"
#include "shapes/3Dshapes/pyramid.h"
#include "shapes/3Dshapes/cube.h"

/******************************* DEFINES *************************************/


/****************************** STRUCTURES ***********************************/


/******************************* FUNCTIONS ***********************************/

extern int coreNB;

double min_d(double d1, double d2, int face, int nt, struct pyramid *p)
{
    if (d1 < d2 || d2 < 0)
        return d1;
    else
    {
        p->inter[nt] = face;
        return d2;
    }
}

struct pyramid *new_pyramid(double x, double y, double z, double h, double s)
{
    struct pyramid *p = malloc(sizeof(struct pyramid));
    p->base = new_vector(x, y, z);
    p->height = h;
    p->base_size = s;
    p->inter = calloc(coreNB, sizeof(char));
    return p;
}

struct pyramid *read_pyramid(FILE *f, unsigned *line, struct material *m)
{
    double x, y, z, h, s, r, g, b;

    if (fscanf(f, "%lg %lg %lg %lg %lg %lg %lg %lg\n",
                &x, &y, &z, &h, &s, &r, &g, &b) != 8)
        errx(3, "Error while parsing input file: line %u", *line);

    ++(*line);

    m->diffuse->r = r;
    m->diffuse->g = g;
    m->diffuse->b = b;
    read_material(f, line, m);
    return new_pyramid(x, y, z, h, s);
}

double intersect_triangle(struct vector *c1, struct vector *c2,
        struct vector *c3, struct ray *r)
{
    struct vector *u = vector_from_points(c1, c2);
    struct vector *v = vector_from_points(c1, c3);
    struct vector *w = new_vector(r->pos->x - c1->x, r->pos->y - c1->y,
            r->pos->z - c1->z);
    struct vector *tmp1 = cross_product(u, v);
    struct vector *tmp2 = cross_product(w, v);
    struct vector *tmp3 = cross_product(u, w);

    double a = dot_product(tmp2, r->dir) / dot_product(tmp1, r->dir);
    double b = dot_product(tmp3, r->dir) / dot_product(tmp1, r->dir);
    double result;

    if (a >= 0 && b >= 0 && a + b <= 1)
        result = -(dot_product(tmp1, w) / dot_product(tmp1, r->dir));
    else
        result = INT_MAX;

    free_vector(u);
    free_vector(v);
    free_vector(w);
    free_vector(tmp1);
    free_vector(tmp2);
    free_vector(tmp3);

    return result;
}

double intersect_pyramid(void *pyramid, struct ray *r, int i)
{
    struct pyramid *p = (struct pyramid *)pyramid;
    double dist = INT_MAX;
    p->inter[i] = 0;

    struct vector *c1 =
        new_vector(p->base->x + p->base_size / 2, p->base->y, p->base->z - p->base_size / 2);
    struct vector *c2 =
        new_vector(p->base->x - p->base_size / 2, p->base->y, p->base->z - p->base_size / 2);
    struct vector *c3 =
        new_vector(p->base->x + p->base_size / 2, p->base->y, p->base->z + p->base_size / 2);
    struct vector *c4 =
        new_vector(p->base->x, p->base->y + p->height, p->base->z);
    struct vector *c5 =
        new_vector(p->base->x - p->base_size / 2, p->base->y, p->base->z + p->base_size / 2);

    dist = min_d(dist, intersect_face(c1, c2, c3, r), 1, i, p);
    dist = min_d(dist, intersect_triangle(c1, c3, c4, r), 2, i, p);
    dist = min_d(dist, intersect_triangle(c1, c2, c4, r), 3, i, p);
    dist = min_d(dist, intersect_triangle(c2, c4, c5, r), 4, i, p);
    dist = min_d(dist, intersect_triangle(c5, c4, c3, r), 5, i, p);

    free_vector(c1);
    free_vector(c2);
    free_vector(c3);
    free_vector(c4);
    free_vector(c5);

    return dist;
}

struct vector *get_normal_pyramid(struct vector *inter_pos, void *pyramid, int i)
{
    struct pyramid *p = (struct pyramid *)pyramid;
    struct vector *c1 =
        new_vector(p->base->x + p->base_size / 2, p->base->y, p->base->z - p->base_size / 2);
    struct vector *c2 =
        new_vector(p->base->x - p->base_size / 2, p->base->y, p->base->z - p->base_size / 2);
    struct vector *c3 =
        new_vector(p->base->x + p->base_size / 2, p->base->y, p->base->z + p->base_size / 2);
    struct vector *c4 =
        new_vector(p->base->x, p->base->y + p->height, p->base->z);
    struct vector *c5 =
        new_vector(p->base->x - p->base_size / 2, p->base->y, p->base->z + p->base_size / 2);
    struct vector *v;

    switch (p->inter[i])
    {
        case 1: v = get_normal_face(c1, c2, c3, inter_pos); break;
        case 2: v = get_normal_face(c1, c3, c4, inter_pos); break;
        case 3: v = get_normal_face(c1, c2, c4, inter_pos); break;
        case 4: v = get_normal_face(c2, c4, c5, inter_pos); break;
        case 5: v = get_normal_face(c5, c4, c3, inter_pos); break;
        default: v = NULL; break;
    }

    free_vector(c1);
    free_vector(c2);
    free_vector(c3);
    free_vector(c4);
    free_vector(c5);

    p->inter[i] = 0;
    return v;
}

void free_pyramid(void *pyramid)
{
    struct pyramid *p = (struct pyramid *)pyramid;
    free_vector(p->base);
    free(p->inter);
    free(p);
}

/******************************** ENDFILE ************************************/
