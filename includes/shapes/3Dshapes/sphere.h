#ifndef SPHERE_H_
#define SPHERE_H_

/****************************** STRUCTURES ***********************************/

struct ray;
struct vector;
struct material *m;

struct sphere
{
    double rayon;
    struct vector *pos;
};

/****************************** PROTOTYPES ***********************************/

//Create and return a new sphere with (xc, yc, zc) center and r sized ray.
struct sphere *new_sphere(double rayon, double xc, double yc, double zc);

//Read parameters of a sphere in a file and return it.
struct sphere *read_sphere(FILE *f, unsigned *line, struct material *m);

//Get the distance between intersection point and ray position.
double intersect_sphere(void *s, struct ray* r, int i);

//Return the normal vector at a point on sphere.
struct vector *get_normal_sphere(struct vector *inter_pos, void *s, int i);

//Free memory of a sphere.
void free_sphere(void *s);

/*****************************************************************************/

#endif /* SPHERE_H_ */
