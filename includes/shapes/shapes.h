#ifndef SHAPES_H_
#define SHAPES_H_

/******************************* INCLUDES ************************************/

#include "3Dshapes/sphere.h"
#include "3Dshapes/cube.h"
#include "3Dshapes/pyramid.h"
#include "2Dshapes/plane.h"

/******************************* STRUCTURES **********************************/

struct vector;
struct ray;
struct material;

//Structure of a shape.
struct shape_list
{
    void *obj;
    struct shape_list *next;
    struct material *m;
    double (*intersect_shape)(void*, struct ray*, int);
    struct vector* (*get_normal_shape)(struct vector*, void*, int);
    void (*free_shape)(void*);
};

/******************************* PROTOTYPES **********************************/

//Create and return a new shape list. The structure id is pointed by obj.
struct shape_list *new_shape_list(void *obj, struct material *m,
        double (*inter)(void*, struct ray*, int),
        struct vector* (*get_normal_shape)(struct vector*, void*, int),
        void (*free_shape)(void*));

//Add a shape in a shape_list.
struct shape_list *add_shape(struct shape_list *sl, void *obj, struct material *m,
        double (*inter)(void*, struct ray*, int),
        struct vector* (*get_normal_shape)(struct vector*, void*, int),
        void (*free_shape)(void*));

//Read the good shape depending on what is parsed in a file.
struct shape_list *read_shapes(FILE *f, unsigned *line, char s[10]);

//Return distance between ray position and intersection point, if there is one.
double intersect_shape(struct shape_list *sl, struct ray *r);

//Get the normal on the intersection point depending on the shape.
struct vector *get_normal_shape(struct shape_list *sl,
        struct vector *inter_pos);

//Free memory of a shape list.
void free_shape_list(struct shape_list *sl);

/*****************************************************************************/

#endif /* SHAPES_H_ */
