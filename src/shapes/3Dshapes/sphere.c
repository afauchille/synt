/******************************** INCLUDES ***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include "tools/vectors.h"
#include "tools/color.h"
#include "ray/ray.h"
#include "shapes/material.h"
#include "ray/camera.h"
#include "shapes/3Dshapes/sphere.h"

/******************************* FUNCTIONS ***********************************/

struct sphere *new_sphere(double rayon, double xc, double yc, double zc)
{
    struct sphere *s = malloc(sizeof(struct sphere));
    s->rayon = rayon;
    s->pos = new_vector(xc, yc, zc);
    return s;
}

struct sphere *read_sphere(FILE *f, unsigned *line, struct material *m)
{
    double rayon, xc, yc, zc;
    double r, g, b;
    if (fscanf(f, "%lg %lg %lg %lg %lg %lg %lg\n",
                &rayon, &xc, &yc, &zc, &r, &g, &b) != 7)
        errx(3, "Error while parsing input file: line %u\n", *line);

    ++(*line);

    m->diffuse->r = r;
    m->diffuse->g = g;
    m->diffuse->b = b;
    read_material(f, line, m);
    return new_sphere(rayon, xc, yc, zc);
}

double intersect_sphere(void *sphere, struct ray *r, int i)
{
    if (i < 0)
        return INT_MAX;

    struct sphere *s = (struct sphere *)sphere;
    double a = pow(r->dir->x, 2) + pow(r->dir->y, 2) + pow(r->dir->z, 2);
    double b = 2 * (r->dir->x * (r->pos->x - s->pos->x) + r->dir->y *
            (r->pos->y - s->pos->y) + r->dir->z * (r->pos->z - s->pos->z));
    double c = pow(r->pos->x - s->pos->x, 2) + pow(r->pos->y - s->pos->y, 2) +
        pow(r->pos->z - s->pos->z, 2) - pow(s->rayon, 2);

    double det = pow(b, 2) - 4 * a * c;

    if (det < 0)
        return INT_MAX;
    else if (det == 0)
        return (-b / (2 * a));
    else
    {
        double t1 = (-b + sqrt(det)) / (2 * a);
        double t2 = (-b - sqrt(det)) / (2 * a);

        return (((t1) > (t2)) ? (t2) : (t1));
    }
}

struct vector *get_normal_sphere(struct vector *inter_pos, void *sphere, int i)
{
    if (i < 0)
        return NULL;

    struct sphere *s = (struct sphere *)sphere;
    struct vector *tmp1 = vector_from_points(s->pos, inter_pos);
    struct vector *normal = normal_vector(tmp1);

    free_vector(tmp1);

    return normal;
}

void free_sphere(void *sphere)
{
    struct sphere *s = (struct sphere *)sphere;
    free_vector(s->pos);
    free(s);
}

/*****************************************************************************/
