/******************************** INCLUDES ***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "tools/vectors.h"
#include "shapes/material.h"
#include "shapes/3Dshapes/cylinder.h"
#include <math.h>
#include <limits.h>
#include "ray/ray.h"
#include "ray/camera.h"

/******************************** FUNCTIONS **********************************/

struct cylinder *new_cylinder(double xs, double ys, double zs, double xe,
        double ye, double ze, double xa, double ya, double za, double rayon,
        double r, double g, double b)
{
    struct vector *tmp;
    struct cylinder *c = malloc(sizeof(struct cylinder));
    c->start = new_vector(xs, ys, zs);
    c->end = new_vector(xe, ye, ze);
    tmp = new_vector(xa, ya, za);
    c->axe = normal_vector(tmp);
    c->rayon = rayon;
    c->m = new_material(r, g, b, 0, 0);

    free_vector(tmp);
    return c;
}

struct cylinder *read_cylinder(FILE *f, unsigned *line)
{
    double xs, ys, zs, xe, ye, ze, xa, ya, za, ray;
    double r, g, b;
    if (fscanf(f, "%lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg",
        &xs, &ys, &zs, &xe, &ye, &ze, &xa, &ya, &za, &ray, &r, &g, &b) != 13)
        errx(3, "Error while parsing input file: line %u\n", *line);

    ++(*line);

    return new_cylinder(xs, ys, zs, xe, ye, ze, xa, ya, za, ray, r, g, b);
}


double intersect_cylinder(struct cylinder *c, struct ray *r)
{
    double a = pow(r->dir->x, 2) + pow(r->dir->y, 2) + pow(r->dir->z, 2);
    double b = 2 * (r->dir->x * (r->pos->x - c->start->x) +
            r->dir->y * (r->pos->y - c->start->y) +
            r->dir->z * (r->pos->z - c->start->z));
    double d = pow(r->pos->x - c->start->x, 2) +
            pow(r->pos->y - c->start->y, 2) + pow(r->pos->z - c->start->z, 2) -
            pow(c->rayon, 2);

    double det = pow(b, 2) - 4 * a * d;

    if (det < 0)
        return INT_MAX;
    else if (det == 0)
        return (-b / (2 * a));
    else
    {
        double t1 = (-b + sqrt(det)) / (2 * a);
        double t2 = (-b - sqrt(det)) / (2 * a);

        if (t1 > t2)
            return t2;
        else
            return t1;
    }
}


void free_cylinder(struct cylinder *c)
{
    free_vector(c->start);
    free_vector(c->end);
    free_vector(c->axe);
    free_material(c->m);
    free(c);
}
/*****************************************************************************/
