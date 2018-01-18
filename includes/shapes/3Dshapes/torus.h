#ifndef _TORUS_H_
#define _TORUS_H_

/******************************* INCLUDES ************************************/



/******************************* DEFINES *************************************/



/****************************** STRUCTURES ***********************************/

struct vector;
struct ray;

struct torus
{
    struct vector *pos;
    double R;
    double r;
};

/****************************** PROTOTYPES ***********************************/

struct torus *new_torus(double x, double y, double z, double R, double r);

struct torus *read_torus(FILE *f, unsigned *line, struct material *m);

double intersect_torus(void *torus, struct ray *r, int i);

struct vector *get_normal_torus(struct vector *inter_pos, void *torus, int i);

void free_torus(void *torus);

/******************************* ENDFILE *************************************/

#endif /* _TORUS_H_ */
