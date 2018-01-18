#ifndef RAY_H_
#define RAY_H_

/****************************** INCLUDES *************************************/

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "camera.h"
#include "../lights/lights.h"
#include "../shapes/shapes.h"

/****************************** STRUCTURES ***********************************/

//Structure of a ray.
struct ray
{
    struct vector *pos;
    double t;
    struct vector *dir;
};

typedef struct ray *t_ray;
struct image;

struct raytracing_param
{
    struct image *img;
    struct camera *c;
    struct shape_list *sl;
    struct light_list *ll;
    struct viewplane *v;
    double anti_cr;
};

/******************************* PROTOTYPES **********************************/

//Create and return a new ray at (x, y, z), pointing at (xdir, ydir, zdir).
struct ray *new_ray(double x, double y, double z, double t, double xdir,
        double ydir, double zdir);

//Free memory of a ray.
void free_ray(struct ray *r);

//Raytracing main algorithm.
void raytracing(struct image *img, struct camera *c,
        struct shape_list *sl, struct light_list *ll, struct viewplane *v,
        double anti_cr, double line, int nt);

struct color *reflect_ray(struct vector *dir, struct vector *pos,
                            struct shape_list *inter_sl, struct shape_list *sl,
                            struct camera *c, struct image *img,
                            struct viewplane *v, struct light_list *ll,
                            int limit);

void reflect_ray_call(struct raytracing_param rp);

//Parse the input file and raytrace the scene.
struct image* raytrace(struct raytracing_param rp);

void raytrace_callback();

/*****************************************************************************/

#endif /* RAY_H_ */
