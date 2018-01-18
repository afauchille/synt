/******************************** INCLUDES ***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include "tools/vectors.h"
#include "ray/camera.h"

/******************************** FUNCTIONS **********************************/

struct viewplane *new_viewplane(double dist, double width, double height)
{
    struct viewplane *v = malloc(sizeof(struct viewplane));
    v->dist = dist;
    v->width = width;
    v->height = height;
    return v;
}

//Return a new camera at coordinates (x, y, z) oriented in (xdir, ydir, zdir).
struct camera *new_camera(double x, double y, double z, double xdir,
        double ydir, double zdir)
{
    struct vector *tmp0, *tmp1;

    struct camera *c = malloc(sizeof(struct camera));
    c->pos = new_vector(x, y, z);
    tmp0 = new_vector(xdir, ydir, zdir);
    tmp1 = vector_from_points(c->pos, tmp0);
    c->dir = normal_vector(tmp1);
    c->up = new_vector(0, 1, 0);
    c->right = new_vector(1, 0, 0);
    c->upleft = NULL;

    free_vector(tmp0);
    free_vector(tmp1);

    return c;
}

//Set the upleft vector of a camera depending on a viewplane.
void set_upleft(struct camera *c, struct viewplane *v)
{
    struct vector *tmp3 = mult_scalar(c->dir, v->dist);
    struct vector *tmp4 = mult_scalar(c->up, v->height / 2);
    struct vector *tmp2 = add_vector(tmp3, tmp4);
    struct vector *tmp7 = mult_scalar(c->right, -(v->width) / 2);
    struct vector *tmp6 = add_vector(tmp7, tmp2);
    c->upleft = add_vector(c->pos, tmp6);

    free_vector(tmp2);
    free_vector(tmp3);
    free_vector(tmp4);
    free_vector(tmp6);
    free_vector(tmp7);
}

//Parse camera coordinates and orientation in a file and return it.
struct camera *read_camera(FILE *f, unsigned *line)
{
    char camera[10];
    double x, y, z, xdir, ydir, zdir;

    if (fscanf(f, "%s\n", camera) != 1 || strcmp(camera, "CAMERA") != 0)
        errx(3, "Error while parsing input file: line %u\n", *line);

    ++(*line);

    if (fscanf(f, "%lg %lg %lg %lg %lg %lg\n",
                &x, &y, &z, &xdir, &ydir, &zdir) != 6)
        errx(3, "Error while parsing input file: line %u\n", *line);

    ++(*line);

    return new_camera(x, y, z, xdir, ydir, zdir);
}

//Free memory of a camera structure.
void free_camera(struct camera *c)
{
    free_vector(c->pos);
    free_vector(c->dir);
    free_vector(c->up);
    free_vector(c->right);
    free_vector(c->upleft);
    free(c);
}

//Free memory of a viewplane structure.
void free_viewplane(struct viewplane *v)
{
    free(v);
}

/*****************************************************************************/
