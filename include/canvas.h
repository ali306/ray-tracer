// canvas.h

// tuples.h

#ifndef CANVAS_H
#define CANVAS_H

#include "tuples.h"

typedef struct
{
    unsigned width;
    unsigned height;
    tuple_t *pixels;
} canvas_t;

canvas_t *canvas(const unsigned width, const unsigned height);

static inline double canvas_clamp(double value)
{
    return fmax(0.0, fmin(1.0, value));
}

void canvas_write_pixel(canvas_t *c, unsigned x, unsigned y, tuple_t color);

tuple_t canvas_pixel_at(const canvas_t *c, const unsigned x, const unsigned y);

char *canvas_to_ppm(const canvas_t *c);

void canvas_free(canvas_t *c);

bool canvas_save(const canvas_t *c, const char *file_path);

#endif
