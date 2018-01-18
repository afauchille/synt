#ifndef _CSG_H_
#define _CSG_H_

struct shape_list;
struct vector;
struct ray;
struct material;

struct csg_shape
{
    struct shape_list *sl;
    char *args;
    int nb_shapes;
    char *inter;
};

struct csg_shape *new_csg_shape(struct shape_list *sl, char *args, int nb);

struct csg_shape *read_csg_shape(FILE *f, unsigned *line, struct material *m);

double intersect_csg(void *obj, struct ray *r, int nt);

struct vector *get_normal_csg(struct vector *inter_pos, void *obj, int nt);

void free_csg(void *obj);

#endif /* _CSG_H_ */
