#ifndef CAMERA_H_
#define CAMERA_H_

struct camera
{
    struct vector *pos;
    struct vector *dir;
    struct vector *up;
    struct vector *right;
    struct vector *upleft;
};

struct viewplane
{
    double dist;
    double width;
    double height;
};

struct viewplane *new_viewplane(double dist, double width, double height);
struct camera *new_camera(double x, double y, double z, double xdir,
        double ydir, double zdir);
void set_upleft(struct camera *c, struct viewplane *v);
struct camera *read_camera(FILE *f, unsigned *line);
void free_camera(struct camera *c);
void free_viewplane(struct viewplane *v);

#endif /* CAMERA_H_ */
