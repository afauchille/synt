#ifndef _PYRAMID_H_
#define _PYRAMID_H_

/******************************* INCLUDES ************************************/



/******************************* DEFINES *************************************/



/****************************** STRUCTURES ***********************************/

struct vector;
struct material;
struct ray;

/* Pyramid */
struct pyramid
{
    struct vector *base;
    double height;
    double base_size;
    char* inter;
};

/****************************** PROTOTYPES ***********************************/

/* Create and return a new pyramid */
struct pyramid *new_pyramid(double x, double y, double z, double h, double s);

struct pyramid *read_pyramid(FILE *f, unsigned *line, struct material *m);

double intersect_pyramid(void *p, struct ray *r, int i);

struct vector *get_normal_pyramid(struct vector *inter_pos, void *p, int i);

void free_pyramid(void *p);

/******************************* ENDFILE *************************************/

#endif /* _PYRAMID_H_ */
