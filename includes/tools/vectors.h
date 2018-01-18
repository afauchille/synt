#ifndef VECTORS_H_
#define VECTORS_H_

/****************************** INCLUDES *************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/****************************** STRUCTURES ***********************************/

//Structure of a vector.
struct vector
{
    double x, y, z;
};

//Typedef of a pointer on vector.
typedef struct vector *t_vector;

/******************************* FUNCTIONS ***********************************/

//Create and return a new vector filled with coordinates in parameters.
static inline
struct vector *new_vector(double x, double y, double z)
{
    struct vector *v = malloc(sizeof(struct vector));
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}

//Return the vector which links two points.
static inline
struct vector *vector_from_points(struct vector *v1, struct vector *v2)
{
    struct vector *v = malloc(sizeof(struct vector));
    v->x = v2->x - v1->x;
    v->y = v2->y - v1->y;
    v->z = v2->z - v1->z;
    return v;
}

//Calculate and return the norm of a vector.
static inline
double norm_vector(struct vector *v)
{
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

static inline
struct vector *normal_of_vector(struct vector *v)
{
    struct vector *newv = malloc(sizeof(struct vector));
    newv->x = -v->y;
    newv->y = v->x;
    newv->z = v->z;
    return newv;
}

//Check if a vector is normalized. Abort otherwise.
static inline
void vector_is_normalized(struct vector *v)
{
    assert(v->x <= 1 && v->x >= 0 &&
            v->y <= 1 && v->y >= 0 &&
            v->z <= 1 && v->z >= 0);
}

//Calculate the normale of a vector and returns it.
static inline
struct vector *normal_vector(struct vector *v)
{
    double norm = norm_vector(v);
    if (norm == 0)
        return NULL;
    struct vector *newv = malloc(sizeof(struct vector));
    newv->x = v->x / norm;
    newv->y = v->y / norm;
    newv->z = v->z / norm;
    return newv;
}
//Calculate and return the addition between two vectors.
static inline
struct vector *add_vector(struct vector *v1, struct vector *v2)
{
    struct vector *v = malloc(sizeof(struct vector));
    v->x = v1->x + v2->x;
    v->y = v1->y + v2->y;
    v->z = v1->z + v2->z;
    return v;
}

//Calculate and return the cross product between two vectors.
static inline
struct vector *cross_product(struct vector *v1, struct vector *v2)
{
    struct vector *v = malloc(sizeof(struct vector));
    v->x = v1->y * v2->z - v1->z * v2->y;
    v->y = v1->z * v2->x - v1->x * v2->z;
    v->z = v1->x * v2->y - v1->y * v2->x;
    return v;
}

//Calculate and return a vector multiplied by a scalar.
static inline
struct vector *mult_scalar(struct vector *v, double d)
{
    struct vector *newv = malloc(sizeof(struct vector));
    newv->x = v->x * d;
    newv->y = v->y * d;
    newv->z = v->z * d;
    return newv;
}

//Calculate and return the dot product between two vectors.
static inline
double dot_product(struct vector *v1, struct vector *v2)
{
    return (double)(v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}

//Display on console the coordinates of a vector.
static inline
void print_vector(struct vector *v)
{
    printf("( x = %f, y = %f, z = %f )\n", v->x, v->y, v->z);
}

//Free the memory of a vector.
static inline
void free_vector(struct vector *v)
{
    free(v);
}

#endif /* VECTORS_H_ */
