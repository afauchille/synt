#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>
#include "tools/vectors.h"
#include "tools/color.h"
#include "shapes/material.h"
#include "ray/ray.h"
#include "shapes/3Dshapes/cube.h"

extern int coreNB;

double min_dist_cube(double d1, double d2, int face, int nt, struct cube *c)
{
    if (d1 < d2 || d2 < 0)
        return d1;
    else
    {
        c->inter[nt] = face;
        return d2;
    }
}

struct cube *new_cube(double x, double y, double z, double size)
{
    struct cube *c = malloc(sizeof(struct cube));
    c->pos = new_vector(x, y, z);
    c->size = size;
    c->c1 = new_vector(x + size / 2, y + size / 2, z - size / 2);
    c->c2 = new_vector(x + size / 2, y + size / 2, z + size / 2);
    c->c3 = new_vector(x + size / 2, y - size / 2, z + size / 2);
    c->c4 = new_vector(x + size / 2, y - size / 2, z - size / 2);
    c->c5 = new_vector(x - size / 2, y - size / 2, z - size / 2);
    c->c6 = new_vector(x - size / 2, y + size / 2, z - size / 2);
    c->c7 = new_vector(x - size / 2, y + size / 2, z + size / 2);
    c->c8 = new_vector(x - size / 2, y - size / 2, z + size / 2);
    c->inter = calloc(coreNB, sizeof(char));
    return c;
}

double intersect_face(struct vector *c1, struct vector *c2, struct vector *c3,
        struct ray *r)
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

    if (a >= 0 && b >= 0 && a <= 1 && b <= 1)
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

double intersect_cube(void *cube, struct ray *r, int i)
{
    struct cube *c = (struct cube *)cube;
    double dist = INT_MAX;

    dist = min_dist_cube(dist, intersect_face(c->c1, c->c2, c->c4, r), 1, i, c);
    dist = min_dist_cube(dist, intersect_face(c->c6, c->c7, c->c5, r), 2, i, c);
    dist = min_dist_cube(dist, intersect_face(c->c1, c->c6, c->c2, r), 3, i, c);
    dist = min_dist_cube(dist, intersect_face(c->c2, c->c7, c->c3, r), 4, i, c);
    dist = min_dist_cube(dist, intersect_face(c->c3, c->c8, c->c4, r), 5, i, c);
    dist = min_dist_cube(dist, intersect_face(c->c4, c->c5, c->c1, r), 6, i, c);

    return dist;
}

struct vector *get_normal_face(struct vector *c1, struct vector *c2,
        struct vector *c3, struct vector *inter_pos)
{
    if (inter_pos == NULL)
        return NULL;

    struct vector *u = vector_from_points(c1, c2);
    struct vector *v = vector_from_points(c1, c3);
    struct vector *w = cross_product(u, v);
    struct vector *x = add_vector(w, inter_pos);
    struct vector *wn = normal_vector(x);

    free_vector(u);
    free_vector(v);
    free_vector(w);
    free_vector(x);

    return wn;
}

struct vector *get_normal_cube(struct vector *inter_pos, void *cube, int i)
{
    struct cube *c = (struct cube *)cube;
    struct vector *v;

    switch (c->inter[i])
    {
        case 1: v = get_normal_face(c->c1, c->c2, c->c4, inter_pos); break;
        case 2: v = get_normal_face(c->c6, c->c7, c->c5, inter_pos); break;
        case 3: v = get_normal_face(c->c1, c->c6, c->c2, inter_pos); break;
        case 4: v = get_normal_face(c->c2, c->c7, c->c3, inter_pos); break;
        case 5: v = get_normal_face(c->c3, c->c8, c->c4, inter_pos); break;
        case 6: v = get_normal_face(c->c4, c->c5, c->c1, inter_pos); break;
        default: v = NULL; break;
    }

    c->inter[i] = 0;
    return v;
}

struct cube *read_cube(FILE *f, unsigned *line, struct material *m)
{
    double x, y, z, s, r, g, b;
    if (fscanf(f, "%lg %lg %lg %lg %lg %lg %lg\n",
                &x, &y, &z, &s, &r, &g, &b) != 7)
        errx(3, "Error while parsing input file: line %u\n", *line);

    ++(*line);

    m->diffuse->r = r;
    m->diffuse->g = g;
    m->diffuse->b = b;
    read_material(f, line, m);
    return new_cube(x, y, z, s);
}


void free_cube(void *cube)
{
    struct cube *c = (struct cube *)cube;

    free_vector(c->pos);
    free_vector(c->c1);
    free_vector(c->c2);
    free_vector(c->c3);
    free_vector(c->c4);
    free_vector(c->c5);
    free_vector(c->c6);
    free_vector(c->c7);
    free_vector(c->c8);
    free(c->inter);
    free(c);
}
