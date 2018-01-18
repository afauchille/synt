#ifndef CUBE_H_
#define CUBE_H_

/*****************************************************************************/

struct vector;
struct ray;
struct material *m;

struct cube
{
    struct vector *pos;
    struct vector *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
    double size;
    char* inter;
};

struct cube *new_cube(double x, double y, double z, double size);

double intersect_face(struct vector *c1, struct vector *c2, struct vector *c3,
        struct ray *r);

double intersect_cube(void *c, struct ray *r, int i);

struct vector *get_normal_face(struct vector *c1, struct vector *c2,
        struct vector *c3, struct vector *inter_pos);

struct vector *get_normal_cube(struct vector *inter_pos, void *c, int i);

struct cube *read_cube(FILE *f, unsigned *line, struct material *m);

void free_cube(void *c);

#endif /* CUBE_H_ */
