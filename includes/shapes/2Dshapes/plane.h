#ifndef PLANE_H_
#define PLANE_H_

/****************************** STRUCTURES ***********************************/

struct ray;
struct vector;
struct material;

//Structure of a plane.
struct plane
{
    struct vector *point;
    struct vector *normal;
    double d;
};

/****************************** PROTOTYPES ***********************************/

//Create and return a new plane at (xp, yp, zp) with normal (xn, yn, zn);
struct plane *new_plane(double xp, double yp, double zp, double xn, double yn,
        double zn);

//Parse and return a plane filled with parameters in file.
struct plane *read_plane(FILE *f, unsigned *line, struct material *m);

//Return distance between ray position and intersection with a plane.
double intersect_plane(void *p, struct ray *r, int i);

//Return the normal at intersection point on a plane.
struct vector *get_normal_plane(struct vector *inter_pos, void *p, int i);

//Free memory of a plane.
void free_plane(void *p);

/*****************************************************************************/

#endif /* PLANE_H_ */
