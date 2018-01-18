/******************************* INCLUDES ************************************/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <limits.h>
#include <math.h>
#include "tools/vectors.h"
#include "shapes/material.h"
#include "tools/color.h"
#include "ray/ray.h"
#include "shapes/3Dshapes/torus.h"

/******************************* DEFINES *************************************/

#define min(x,y)        (((x) > (y)) ? (y) : (x))
#define min_pos(x,y)    ((((x) > (y)) && ((y) > (0))) ? (y) : (x))
#define absolute(x)     (((x) < (0)) ? (-x) : (x))
#define PI              3.14159265359
#define EPSILON         0.0000000001

/******************************* FUNCTIONS ***********************************/

struct torus *new_torus(double x, double y, double z, double R, double r)
{
    struct torus *t = malloc(sizeof(struct torus));
    t->pos = new_vector(x, y, z);
    t->R = R;
    t->r = r;
    return t;
}

struct torus *read_torus(FILE *f, unsigned *line, struct material *m)
{
    double x, y, z, lr, br, r, g, b;

    if (fscanf(f, "%lg %lg %lg %lg %lg %lg %lg %lg\n",
                &x, &y, &z, &br, &lr, &r, &g, &b) != 8)
        errx(3, "Error while parsing the input file: line %u", *line);

    ++(*line);

    m->diffuse->r = r;
    m->diffuse->g = g;
    m->diffuse->b = b;
    read_material(f, line, m);
    return new_torus(x, y, z, br, lr);
}

double resolve_second_degree(double a, double b, double c)
{
    double d;

    d = b * b - 4 * a * c;

    if (d == 0)
        return -b / (2 * a);

    else if (d < 0)
        return INT_MAX;

    else
        return min((-b - sqrt(d)) / (2 * a), (-b + sqrt(d)) / (2 * a));
}

double resolve_third_degree(double a, double b, double c, double d)
{
    double q, r, D, Dprime, t, s, X;

    b = b / a;
    c = c / a;
    d = d / a;

    X = INT_MAX;
    q = (b * b - 3 * c) / 9.0;
    r = (b * (b * b - 4.5 * c) + 13.5 * d) / 27.0;
    D = q * q * q - r * r;

    if (D >= 0)
    {
        Dprime = r / exp((3.0 / 2.0) * log(q));
        t = acos(Dprime) / 3;
        s = -2 * sqrt(q);

        for (int i = 0; i < 3; ++i)
            X = min(X, s * cos(t + 2 * PI * (i / 3.0)) - b / 3.0);
    }

    else
    {
        s = exp((1.0 / 3.0) * log(sqrt(-D) + absolute(r)));

        if (r < 0)
            X = s + q / s - b / 3.0;
        else
            X = - s - q / s - b / 3.0;
    }

    return X;
}

double resolve_fourth_degree(double a, double b, double c, double d, double e)
{

    if (a == 0 && b == 0)
        return resolve_second_degree(c, d, e);

    else if (a == 0)
        return resolve_third_degree(b, c, d, e);

    b = b / a;
    c = c / a;
    d = d / a;
    e = e / a;

    if (e == 0)
        return min(0, resolve_third_degree(1, b, c, d));

    else
    {
        double p = -0.375 * b * b + c;
        double q = 0.125 * b * b * b - 0.5 * b * c + d;
        double r = -0.01171875 * b * b * b * b + 0.0625 * b * b * c - 0.25 * b * d + e;
        double z0 = resolve_third_degree(1, -0.5 * p, -r, 0.5 * r * p - 0.125 * q * q);

        if (z0 == INT_MAX)
            return INT_MAX;

        double d1 = 2.0 * z0 - p;
        double d2;

        if (d1 < 0)
        {
            if (d1 > -EPSILON)
                d1 = 0;
            else
                return INT_MAX;
        }

        if (d1 < EPSILON)
        {
            d2 = z0 * z0 - r;

            if (d2 < 0)
                return INT_MAX;

            else
                d2 = sqrt(d2);
        }

        else
        {
            d1 = sqrt(d1);
            d2 = 0.5 * q / d1;
        }

        p = d1 * d1 - 4.0 * (z0 - d2);
        double X = INT_MAX;

        if (p == 0)
            X = min(X, -0.5 * d1 + 0.25 * b);

        else if (p > 0)
        {
            p = sqrt(p);
            X = min(X, -0.5 * (d1 + p) - 0.25 * b);
            X = min(X, -0.5 * (d1 - p) - 0.25 * b);
        }

        p = d1 * d1 - 4.0 * (z0 + d2);

        if (p == 0)
            X = min(X, 0.5 * d1 + 0.25 * b);

        else if (p > 0)
        {
            p = sqrt(p);
            X = min(X, 0.5 * (d1 + p) - 0.25 * b);
            X = min(X, 0.5 * (d1 - p) - 0.25 * b);
        }

        return X;
    }
}

double intersect_torus(void *torus, struct ray *ray, int i)
{
    if (i < 0)
        return INT_MAX;

    struct torus *t = (struct torus*)torus;

    double d0 = ray->dir->x;
    double d1 = ray->dir->y;
    double d2 = ray->dir->z;
    double p0 = ray->pos->x;
    double p1 = ray->pos->y;
    double p2 = ray->pos->z;
    double R = t->R;
    double r = t->r;

    double a = pow(d0 * d0 + d1 * d1 + d2 * d2, 2);
    double b = 4 * (d0 * d0 + d1 * d1 + d2 * d2) * (d0 * p0 + d1 * p1 + d2 * p2);
    double c = 2 * (d0 * d0 + d1 * d1 + d2 * d2) * (p0 * p0 + p1 * p1 + p2 * p2 +
            R * R - r * r) + 4 * pow(d0 * p0 + d1 * p1 + d2 * p2, 2) - 4 * R * R *
        (d0 * d0 + d2 * d2);
    double d = 4 * (d0 * p0 + d1 * p1 + d2 * p2) * (p0 * p0 + p1 * p1 + p2 * p2 +
            R * R - r * r) - 8 * R * R * (d0 * p0 + d2 * p2);
    double e = pow(p0 * p0 + p1 * p1 + p2 * p2 + R * R - r * r, 2) - 4 * R * R *
        (p0 * p0 + p2 * p2);

    return resolve_fourth_degree(a, b, c, d, e);
}

struct vector *get_normal_torus(struct vector *inter_pos, void *torus, int i)
{
    if (i < 0)
        return NULL;

    struct torus *t = (struct torus*)torus;
    double x = inter_pos->x;
    double y = inter_pos->y;
    double z = inter_pos->z;

    double dx = 4.0 * x * (x * x + y * y + z * z + t->R * t->R - t->r * t->r) -
            8.0 * t->R * t->R * x;

    double dy = 4.0 * y * (x * x + y * y + z * z + t->R * t->R - t->r * t->r);

    double dz = 4.0 * z * (x * x + y * y + z * z + t->R * t->R - t->r * t->r) -
            8.0 * t->R * t->R * z;

    struct vector *tmp1 = new_vector(dx, dy, dz);
    struct vector *tmp2 = normal_vector(tmp1);

    free_vector(tmp1);

    return tmp2;
}

void free_torus(void *torus)
{
    struct torus *t = (struct torus*)torus;
    free_vector(t->pos);
    free(t);
}

/******************************** ENDFILE ************************************/

