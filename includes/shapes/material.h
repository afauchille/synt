#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <SDL/SDL_image.h>
#include <SDL/SDL.h>
#include "../tools/image.h"
#include "../ray/camera.h"
/******************************* STRUCTURES **********************************/

struct material
{
    struct color *diffuse;
    int bump_mapped;
    int textured;
    struct vector *v;
};

/******************************** PROTOTYPES *********************************/

struct material *new_material(double r, double g, double b, int bp, int t);

void set_material(struct material *m, double r, double g, double b, int bp,
                                            int t);

void read_material(FILE *f, unsigned *line, struct material *m);

void free_material(struct material *m);

void apply_texture(struct color *c, struct material *m,
            struct vector *inter_dir, struct image *img, double x, double y);

struct vector *apply_mapping(struct vector *v, struct material *m);

double noise(struct vector *v);

void init(void);

void generateNoise();

double smoothNoise(double x, double y);

double turbulence(double x, double y, double size);

/*****************************************************************************/

#endif /* MATERIAL_H_ */
