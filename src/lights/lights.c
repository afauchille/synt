/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <string.h>
#include <err.h>
#include "tools/vectors.h"
#include "tools/color.h"
#include "tools/image.h"
#include "shapes/material.h"
#include "shapes/shapes.h"
#include "ray/ray.h"
#include "lights/lights.h"

/*****************************************************************************/

struct light_list *new_light_list(double x, double y, double z, double r,
        double g, double b)
{
    struct light_list *ll = malloc(sizeof(struct light_list));
    ll->r = r;
    ll->b = b;
    ll->g = g;
    ll->pos = new_vector(x, y, z);
    ll->next = NULL;
    return ll;
}

struct light_list *add_light(struct light_list *ll, unsigned id,
        struct light_list *new)
{
    struct light_list *tmp = ll;
    new->id = id;

    if (ll == NULL)
        return new;
    else
    {
        while (tmp->next)
            tmp = tmp->next;

        tmp->next = new;
        return ll;
    }
}

struct light_list *read_lights(FILE *f, unsigned *line, char s[10])
{
    char light[10];
    struct light_list *ll = NULL;
    double x, y, z, r, g, b;

    if (fscanf(f, "%lg %lg %lg %lg %lg %lg\n",
            &x, &y, &z, &r, &g, &b) != 6)
        errx(3, "Error while parsing input file: line %u", *line);

    if (strcmp(s, "DIFFUSE") == 0)
        ll = add_light(ll, 0, new_light_list(x, y, z, r, g, b));
    else if (strcmp(s, "AMBIANT") == 0)
        ll = add_light(ll, 1, new_light_list(x, y, z, r, g, b));

    while (1)
    {
        if (fscanf(f, "%s\n", light) != 1)
            break;

        ++(*line);

        if (fscanf(f, "%lg %lg %lg %lg %lg %lg\n",
                &x, &y, &z, &r, &g, &b) != 6)
            errx(3, "Error while parsing input file: line %u", *line);

        ++(*line);

        if (strcmp(light, "DIFFUSE") == 0)
            ll = add_light(ll, 0, new_light_list(x, y, z, r, g, b));
        else if (strcmp(light, "AMBIANT") == 0)
            ll = add_light(ll, 1, new_light_list(x, y, z, r, g, b));
        else
            break;
    }

    return ll;
}

void free_light_list(struct light_list *ll)
{
    if (ll->next != NULL)
        free_light_list(ll->next);

    free_vector(ll->pos);
    free(ll);
}

void get_light(struct vector *dir, struct vector *pos, struct shape_list *sl,
        struct light_list *ll, struct color *c)
{
    struct vector *light_vector = vector_from_points(pos, ll->pos);
    struct vector *light_normal = normal_vector(light_vector);
    double angle = dot_product(dir, light_normal);

    if (angle > 0)
    {
        c->r += sl->m->diffuse->r * ll->r * angle;
        c->g += sl->m->diffuse->g * ll->g * angle;
        c->b += sl->m->diffuse->b * ll->b * angle;
    }

    normalize_color(c);
    free_vector(light_vector);
    free_vector(light_normal);
}

/*****************************************************************************/
