/***************************** INCLUDES **************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "tools/image.h"
#include "lights/lights.h"
#include "shapes/shapes.h"
#include "ray/camera.h"
#include "parser/parser.h"

/***************************** FUNCTIONS *************************************/

//Open a file at path in parameter for parsing it.
FILE *open_for_parse(char *path)
{
    FILE *f = fopen(path, "r");

    if (f == NULL)
        errx(3, "Error while opening %s\n", path);

    return f;
}

//Close a file.
void close_file(FILE *f)
{
    if (f == NULL)
        errx(3, "Error while closing file\n");

    fclose(f);
}

void read_options(FILE *f, unsigned *line, unsigned *nbcore, double *anti_cr,
		  unsigned *fps, unsigned *sec)
{
    char options[10];

    if (fscanf(f, "%s\n", options) != 1)
        errx(3, "Error while parsing input file: line %u", *line);

    if (strcmp(options, "OPTIONS") != 0)
        errx(3, "Error while parsing input file: line %u", *line);

    ++(*line);

    if (fscanf(f, "%u %lg %u %u\n", nbcore, anti_cr, fps, sec) != 4)
        errx(3, "Error while parsing input file: line %u", *line);

    ++(*line);
}

//Open, parse, create what is needed and close the file at path.
void parse_file(char *path, struct camera **c, struct shape_list **sl,
		struct light_list **ll, struct image *img, unsigned *nbcore,
		double *anti_cr, unsigned *fps, unsigned *sec)
{
    unsigned line = 1;
    FILE *f = open_for_parse(path);
    char str[10];

    read_options(f, &line, nbcore, anti_cr, fps, sec);
    read_device(f, &line, img);
    (*c) = read_camera(f, &line);
    (*sl) = read_shapes(f, &line, str);
    (*ll) = read_lights(f, &line, str);

    close_file(f);
}
