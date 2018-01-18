#ifndef LIGHTS_H_
#define LIGHTS_H_

/*****************************************************************************/

struct color;
struct vector;
struct shape_list;

struct light_list
{
    unsigned id;
    double r, g, b;
    struct vector *pos;
    struct light_list *next;
};

/*****************************************************************************/

struct light_list *new_light_list(double x, double y, double z, double r,
        double g, double b);

void free_light_list(struct light_list *ll);

void get_light(struct vector *dir, struct vector *pos, struct shape_list *sl,
        struct light_list *ll, struct color *c);

/*void apply_shadow(struct light_list *ll, struct shape_list *sl,
        struct shape_list *inter_sl, struct vector *inter_pos,struct color *c);*/

struct light_list *read_lights(FILE *f, unsigned *line, char s[10]);

/*****************************************************************************/

#endif /* LIGHTS_H_ */
