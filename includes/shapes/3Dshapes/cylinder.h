/******************************* STRUCTURES **********************************/

struct vector;
struct material;
struct ray;

struct cylinder
{
    struct vector *start; /* point */
    struct vector *end; /* point */
    struct vector *axe; /* vector */
    double rayon;
    struct material *m;
};

/******************************** PROTOTYPES *********************************/

//Create and return a new cylinder.
struct cylinder *new_cylinder(double xs, double ys, double zs, double xe,
        double ye, double ze, double xa, double ya, double za, double rayon,
        double r, double g, double b);

//Parse input file and return new cylinder filled with parameters.
struct cylinder *read_cylinder(FILE *f, unsigned *line);

double intersect_cylinder (struct cylinder *c, struct ray *r);

//Free memory of a cylinder.
void free_cylinder(struct cylinder *c);

/*****************************************************************************/
