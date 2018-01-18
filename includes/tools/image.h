#ifndef IMAGE_H_
#define IMAGE_H_

/******************************* INCLUDES ************************************/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "color.h"

/******************************* STRUCTURES **********************************/

struct pixel
{
    unsigned char r, g, b;
};

struct image
{
    unsigned w, h;
    struct pixel *pixels;
};

/**************************** IMAGE TREATMENT ********************************/

static inline
GdkPixbuf* image_to_GdkPixbuf(struct image *img)
{
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data((guchar *)img->pixels,
        GDK_COLORSPACE_RGB, FALSE, 8, img->w, img->h, img->w * 3, NULL, NULL);

    //return gtk_image_new_from_pixbuf(pixbuf);
    return pixbuf;
}

static inline
void free_image(struct image* img)
{
    if (img)
    {
	if (img->pixels)
	    free(img->pixels);
	free(img);
    }
}

//Display an image on the screen.
/*static inline
SDL_Surface *display_image_SDL(SDL_Surface *img)
{
    SDL_Surface *screen =
        SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);

    if (!screen)
        errx(1, "Could not set %dx%d video mode: %s\n", img->w, img->h,
                SDL_GetError());

    if (SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    SDL_UpdateRect(screen, 0, 0, img->w, img->h);
    return screen;
}

//Create and return a new surface with format wxh.
static inline
SDL_Surface *create_surface(int w, int h)
{
    Uint32 rmask, gmask, bmask, amask;

    amask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    rmask = 0xff0000ff;

    return SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
}*/

//Parse surface format in a file and return it.
static inline
void read_device(FILE *f, unsigned *line, struct image *img)
{
    char device[10];
    unsigned w, h;

    if (fscanf(f, "%s\n", device) != 1 || strcmp(device, "DEVICE") != 0)
        errx(3, "Error while parsing input file: line %u\n", *line);

    ++(*line);

    if (fscanf(f, "%u %u\n", &w, &h) != 2)
        errx(3, "Error while parsing input file: line %u\n", *line);

    ++(*line);

    img->w = w;
    img->h = h;
    //return create_surface(w, h);
}

/****************************** PIXEL TREATMENT ******************************/

static inline
void set_pixel_img(struct image *img, double r, double g, double b, unsigned x,
        unsigned y)
{
    size_t place = y * img->w + x;
    double rc, gc, bc;

    rc = r * 255;
    gc = g * 255;
    bc = b * 255;

    img->pixels[place].r = (unsigned char)rc;
    img->pixels[place].g = (unsigned char)gc;
    img->pixels[place].b = (unsigned char)bc;
}

//Return the pixel at coordinates (x, y) in an image.
/*static inline
Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
        case 1: return *p; break;
        case 2: return *(Uint16*)p; break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;
        case 4: return *(Uint32*)p; break;
        default: return 0;
    }
}

//Set a pixel at coordinates (x, y) in an image.
static inline
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1: *p = pixel; break;
        case 2: *(Uint16*)p = pixel; break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4: *(Uint32*)p = pixel; break;
    }
    }

//Set a color (r, g, b) on an image at coordinates (x, y).
static inline
void set_color(SDL_Surface *surface, int x, int y, struct color *c)
{
    struct color *c_255 = new_color(255, 255, 255);
    struct color *fc = mult_color(c, c_255);

    Uint32 pixel = get_pixel(surface, x, y);
    pixel = SDL_MapRGB(surface->format, (Uint8)fc->r, (Uint8)fc->g,
            (Uint8)fc->b);
    set_pixel(surface, x, y, pixel);

    free_color(fc);
    free_color(c_255);
}*/

/*****************************************************************************/

#endif /* IMAGE_H_ */
