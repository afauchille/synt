#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <err.h>
#include "shapes/shapes.h"
#include "shapes/3Dshapes/csg.h"
#include "ray/ray.h"
#include "tools/vectors.h"
#include "shapes/material.h"

extern int coreNB;

double min_dist_csg(double d1, double d2, int nt, int i, struct csg_shape *cs)
{
    if (d1 < d2)
    {
        cs->inter[nt] = i;
        return d1;
    }

    else
    {
        cs->inter[nt] = i + 1;
        return d2;
    }
}

struct csg_shape *new_csg_shape(struct shape_list *sl, char *args, int nb)
{
    struct csg_shape *cs = malloc(sizeof(struct csg_shape));
    cs->nb_shapes = nb;
    cs->args = args;
    cs->sl = sl;
    cs->inter = calloc(coreNB, sizeof(char));
    return cs;
}

char *read_csg_args(FILE *f, unsigned *line, int nb_shapes)
{
    char *args = calloc(nb_shapes - 1, sizeof(char));

    if (fscanf(f, "%s\n", args) != 1)
        errx(3, "Error while parsing input file: line %u", *line);

    ++(*line);

    return args;
}

struct csg_shape *read_csg_shape(FILE *f, unsigned *line, struct material *m)
{
    struct shape_list *sl = NULL;
    char s[10];
    char *args;
    int nb_shapes;
    double r, g, b;

    if (fscanf(f, "%d\n", &nb_shapes) != 1)
        errx(3, "Error while parsing input file: line %u", *line);

    ++(*line);

    if (fscanf(f, "%lg %lg %lg\n", &r, &g, &b) != 3)
        errx(3, "Error while parsing input file: line %u", *line);

    ++(*line);

    m->diffuse->r = r;
    m->diffuse->b = b;
    m->diffuse->g = g;

    read_material(f, line, m);
    sl = read_shapes(f, line, s);
    args = read_csg_args(f, line, nb_shapes);

    if (fscanf(f, "%s\n", s) != 1)
        errx(3, "Error while parsing input file: line %u", *line);

    ++(*line);

    if (strcmp(s, "ENDCSG") != 0)
        errx(3, "No ENDCSG");

    return new_csg_shape(sl, args, nb_shapes);
}

double intersect_csg(void *obj, struct ray *r, int nt)
{
    struct csg_shape *cs = (struct csg_shape*)obj;
    double dists[cs->nb_shapes];
    double min_dist;
    struct shape_list *tmp_sl = cs->sl;

    for (int i = 0; i < cs->nb_shapes; ++i)
    {
        dists[i] = tmp_sl->intersect_shape(tmp_sl->obj, r, nt);
        tmp_sl = tmp_sl->next;
    }

    min_dist = dists[0];

    for (int i = 0; i < cs->nb_shapes - 1; ++i)
    {
        if(cs->args[i] == 'U')
            min_dist = min_dist_csg(dists[i], dists[i + 1], nt, i, cs);

        else if(cs->args[i] == 'I')
        {
            if (dists[i] == INT_MAX || dists[i + 1] == INT_MAX)
                min_dist = INT_MAX;
            else
                min_dist = min_dist_csg(dists[i], dists[i + 1], nt, i, cs);
        }

        else
        {
            if(dists[i] == INT_MAX)
                min_dist = INT_MAX;
            else if(dists[i + 1] == INT_MAX)
            {
                min_dist = dists[i];
                cs->inter[nt] = i;
            }
            else
                min_dist = INT_MAX;
        }

        dists[i + 1] = min_dist;
    }

    return min_dist;
}

struct vector *get_normal_csg(struct vector *inter_pos, void *obj, int nt)
{
    if (!inter_pos)
        return NULL;

    struct csg_shape *cs = (struct csg_shape*)obj;
    struct shape_list *tmp_sl = cs->sl;

    for (int i = 0; i < cs->inter[nt]; ++i)
        tmp_sl = tmp_sl->next;

    return tmp_sl->get_normal_shape(inter_pos, tmp_sl->obj, nt);
}

void free_csg(void *obj)
{
    if (!obj)
        return;
}
