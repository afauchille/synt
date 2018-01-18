/****************************** INCLUDES *************************************/

#include <stdio.h>
#include <stdlib.h>
#include "../../includes/tools/color.h"
#include "../../includes/shapes/material.h"
#include "../../includes/tools/vectors.h"
#include <math.h>
#include <time.h>
#include <err.h>
#include <string.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL.h>
#include "../../includes/tools/image.h"

/****************************** FUNCTIONS ************************************/

struct material *new_material(double r, double g, double b, int bp, int t)
{
    struct material *m = malloc(sizeof(struct material));
    m->diffuse = new_color(r, g, b);
    m->bump_mapped = bp;
    m->textured = t;
    return m;
}

void set_material(struct material *m, double r, double g, double b, int bp,
                                                                        int t)
{
    m->diffuse->r = r;
    m->diffuse->g = g;
    m->diffuse->b = b;
    m->bump_mapped = bp;
    m->textured = t;
}

void read_material(FILE *f, unsigned *line, struct material *m)
{
    char t[10], bm[10];

    if (fscanf(f, "%s %s\n", t, bm) != 2)
        errx(3, "Error while parsing input file: line %u", *line);

    if (strncmp(t, "NONE", 4) == 0)
        m->textured = 0;
    else if (strncmp(t, "WOOD", 4) == 0)
        m->textured = 1;
    else if (strncmp(t, "MARBLE1", 7) == 0)
        m->textured = 2;
    else if (strncmp(t, "MARBLE2", 7) == 0)
        m->textured = 3;
    else if (strncmp(t, "REFLE", 5) == 0)
        m->textured = 4;
    else
        errx(3, "Error while parsing input file: line %u", *line);

    if (strncmp(bm, "NONE", 4) == 0)
        m->bump_mapped = 0;
    else if (strncmp(bm, "BM1", 3) == 0)
        m->bump_mapped = 1;
    else if (strncmp(bm, "BM2", 3) == 0)
        m->bump_mapped = 2;
    else if (strncmp(bm, "BM3", 3) == 0)
        m->bump_mapped = 3;
    else if (strncmp(bm, "BM4", 3) == 0)
        m->bump_mapped = 4;
    else if (strncmp(bm, "BM5", 3) == 0)
        m->bump_mapped = 5;
    else if (strncmp(bm, "BM6", 3) == 0)
        m->bump_mapped = 6;
    else
        errx(3, "Error while parsing input file: line %u", *line);

    ++(*line);
}

static int start3 = 1;

void apply_texture(struct color *c, struct material *m,
            struct vector *inter_dir, struct image *img, double x, double y)
{
    // Wood texture
    if (m->textured == 1)
    {
        if (start3)
        {
            m->diffuse->r = 0.42;
            m->diffuse->g = 0.282;
            m->diffuse->b = 0;
        }

        struct vector *new = new_vector(0,0,0);

        double noise1 = 20 * noise(inter_dir);
        new->x = noise1 - floor(noise1);
        new->y = noise1 - floor(noise1);
        new->z = noise1 - floor(noise1);

        inter_dir->x = inter_dir->x + new->x;
        inter_dir->y = inter_dir->y + new->y;
        inter_dir->z = inter_dir->z + new->z;
    }
    // Marble1 texture
    else if (m->textured == 2)
    {
        double xPeriod = 0.0;
        double yPeriod = 1.0;
        double turbPower = 4.0;
        double turbSize = 110.0;
        double xyValue;
        double sineValue;

        xyValue = x * xPeriod / img->h + y * yPeriod / img->w
                + turbPower * turbulence(x, y, turbSize) / 64.0;
        sineValue = fabs(sin(xyValue));

        c->r = sineValue;
        c->g = sineValue;
        c->b = sineValue;
    }
    // Marble2 texture
    else if (m->textured == 3)
    {
        double xPeriod = 0.0;
        double yPeriod = 0.001;
        double turbPower = 5.0;
        double turbSize = 37.0;
        double xyValue;
        double sineValue;

        xyValue = x * xPeriod / img->h + y * yPeriod / img->w
                + turbPower * turbulence(x, y, turbSize) / 64.0;
        sineValue = fabs(sin(xyValue));

        c->r = sineValue;
        c->g = sineValue;
        c->b = sineValue;
    }
}

struct vector *apply_mapping(struct vector *v, struct material *m)
{
    if (m->bump_mapped == 1)
    {
        struct vector *new = new_vector(0,0,0);

        double sum = 0;

        for (int i = 1; i < 9; i++)
            sum += fabs(noise(v));

        new->x = 1 / sum;
        new->y = 1 / sum;
        new->z = 1 / sum;

        v = add_vector(v, new);

        return v;
    }
    else if (m->bump_mapped == 2)
    {
        struct vector *new = new_vector(0,0,0);

        double sum = 0;

        for (int i = 1; i < 9; i++)
            sum += fabs(noise(v));

        new->x = sum;
        new->y = sum;
        new->z = sum;

        v = add_vector(v, new);

        return v;
    }
    else if (m->bump_mapped == 3)
    {
        struct vector *new = new_vector(0,0,0);

        double sum = 0;

        for (int i = 1; i < 9; i++)
            sum += noise(v);

        new->x = sin(v->x + sum);
        new->y = sin(v->x + sum);
        new->z = sin(v->x + sum);

        v = add_vector(v, new);

        return v;
    }
    else if (m->bump_mapped == 4)
    {
        struct vector *new = new_vector(0,0,0);

        double sum = 0;

        for (int i = 1; i < 9; i++)
            sum += fabs(noise(v));

        new->x = sin(v->x + 1 / sum);
        new->y = sin(v->x + 1 / sum);
        new->z = sin(v->x + 1 / sum);

        v = add_vector(v, new);

        return v;
    }
    else if (m->bump_mapped == 5)
    {
        struct vector *new = new_vector(0,0,0);

        double sum = 0;

        for (int i = 1; i < 9; i++)
            sum += fabs(noise(v));

        new->x = sin(v->x + sum);
        new->y = sin(v->x + sum);
        new->z = sin(v->x + sum);

        v = add_vector(v, new);

        return v;
    }
    else if (m->bump_mapped == 6)
    {
        struct vector *new = new_vector(0,0,0);

        double sum = 0;

        for (int i = 0; i < 8; i++)
            sum += fabs(noise(v)/* / 2 */);

        new->x = sum;
        new->y = sum;
        new->z = sum;

        v = add_vector(v, new);

        return v;
    }
    else
        return v;
}

void free_material(struct material *m)
{
    free_color(m->diffuse);
    free(m);
}

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static int start = 1;

#define s_curve(t) ( t * t * (3. - 2. * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
    t = vec[i] + N;\
    b0 = ((int)t) & BM;\
    b1 = (b0+1) & BM;\
    r0 = t - (int)t;\
    r1 = r0 - 1.;

double noise(struct vector *vector)
{
    double vec[3];
    vec[0] = vector->x;
    vec[1] = vector->y;
    vec[2] = vector->z;

    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
    int i, j;

    if (start)
    {
        start = 0;
        init();
    }

    setup(0, bx0,bx1, rx0,rx1);
    setup(1, by0,by1, ry0,ry1);
    setup(2, bz0,bz1, rz0,rz1);

    i = p[ bx0 ];
    j = p[ bx1 ];

    b00 = p[ i + by0 ];
    b10 = p[ j + by0 ];
    b01 = p[ i + by1 ];
    b11 = p[ j + by1 ];

    t  = s_curve(rx0);
    sy = s_curve(ry0);
    sz = s_curve(rz0);

    #define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

    q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
    q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
    a = lerp(t, u, v);

    q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
    q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
    b = lerp(t, u, v);

    c = lerp(sy, a, b);

    q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
    q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
    a = lerp(t, u, v);

    q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
    q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
    b = lerp(t, u, v);

    d = lerp(sy, a, b);

    return lerp(sz, c, d);
}

static void normalize(float v[3])
{
    float s;

    s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
    v[2] = v[2] / s;
}

void init(void)
{
    int i, j, k;

    for (i = 0 ; i < B ; i++)
    {
        p[i] = i;

        for (j = 0 ; j < 3 ; j++)
            g3[i][j] = (float)((rand() % (B + B)) - B) / B;
        normalize(g3[i]);
    }

    while (--i)
    {
        k = p[i];
        p[i] = p[j = rand() % B];
        p[j] = k;
    }

    for (i = 0 ; i < B + 2 ; i++)
    {
        p[B + i] = p[i];
        for (j = 0 ; j < 3 ; j++)
            g3[B + i][j] = g3[i][j];
    }
}

#define noiseHeight 500
#define noiseWidth 500
double noisetab[noiseWidth][noiseHeight];
static int start2 = 1;

void generateNoise()
{
    for (int x = 0; x < noiseWidth; x++)
    {
        for (int y = 0; y < noiseHeight; y++)
            noisetab[x][y] = (rand() % 32768) / 32768.0;
    }
}

double smoothNoise(double x, double y)
{
    if (start2)
    {
        generateNoise();
        start2 = 0;
    }
    //get fractional part of x and y
    double fractX = x - (int)x;
    double fractY = y - (int)y;
    //generateNoise();
    //wrap around
    int x1 = ((int)x + noiseWidth) % noiseWidth;
    int y1 = ((int)y + noiseHeight) % noiseHeight;

    //neighbor values
    int x2 = (x1 + noiseWidth - 1) % noiseWidth;
    int y2 = (y1 + noiseHeight - 1) % noiseHeight;
    // smooth the noise with bilinear interpolation
    double value = 0.0;
    value += fractX * fractY * noisetab[x1][y1];
    value += fractX * (1 - fractY) * noisetab[x1][y2];
    value += (1 - fractX) * fractY * noisetab[x2][y1];
    value += (1 - fractX) * (1 - fractY) * noisetab[x2][y2];

    return value;
}

double turbulence(double x, double y, double size)
{
    double value = 0.0;
    double s_size = size;

    while (size >= 1)
    {
        value += smoothNoise(x / size, y / size) * size;
        size /= 2.0;
    }

    return (128.0 * value / s_size);
}
/*****************************************************************************/
