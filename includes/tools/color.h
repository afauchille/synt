#ifndef COLOR_H_
#define COLOR_H_

/******************************** INCLUDES ***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>

/******************************* STRUCTURES **********************************/

//Structure of a color.
struct color
{
    double r, g, b;
};

/******************************** FUNCTIONS **********************************/

//Create and return a new color filled with elements in parameters.
static inline
struct color *new_color(double r, double g, double b)
{
    struct color *c = malloc(sizeof(struct color));
    c->r = r;
    c->g = g;
    c->b = b;
    return c;
}

//Calculate and return addition between two colors.
static inline
struct color *add_color(struct color *c1, struct color *c2)
{
    struct color *c = malloc(sizeof(struct color));
    c->r = c1->r + c2->r;
    c->g = c1->g + c2->g;
    c->b = c1->b + c2->b;
    return c;
}

//Calculate addition between two colors in the first one.
static inline
void void_add_color(struct color *c1, struct color *c2)
{
    c1->r += c2->r;
    c1->g += c2->g;
    c1->b += c2->b;
}

//Calculate and return substraction between two colors.
static inline
struct color *sub_color(struct color *c1, struct color *c2)
{
    struct color *c = malloc(sizeof(struct color));
    c->r = c1->r - c2->r;
    c->g = c1->g - c2->g;
    c->b = c1->b - c2->b;

    if (c->r < 0 || c->g < 0 || c->b < 0)
        errx(3, "Error: parameter(s) of color inferior to 0");

    return c;
}

//Calculate and return product between two colors.
static inline
struct color *mult_color(struct color *c1, struct color *c2)
{
    struct color *c = malloc(sizeof(struct color));
    c->r = c1->r * c2->r;
    c->g = c1->g * c2->g;
    c->b = c1->b * c2->b;
    return c;
}

//Calculate the division of a color by a number.
static inline
void void_div_scalar_color(struct color *c, double d)
{
    if (d != 0)
    {
        c->r /= d;
        c->g /= d;
        c->b /= d;
    }
}

//Adjust a color.
static inline
void normalize_color(struct color *c)
{
    if (c->r > 1)
        c->r = 1;
    if (c->g > 1)
        c->g = 1;
    if (c->b > 1)
        c->b = 1;
}

static inline
void reverse_normalize_color(struct color *c)
{
    c->r /= 255;
    c->g /= 255;
    c->b /= 255;
}

//Free memory of a color.
static inline
void free_color(struct color *c)
{
    free(c);
}

/*****************************************************************************/

#endif /* COLOR_H */
