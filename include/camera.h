// camera.h

#ifndef CAMERA_H
#define CAMERA_H

#include "canvas.h"
#include "matrices.h"
#include "rays.h"
#include "world.h"

typedef struct
{
    unsigned hsize;
    unsigned vsize;
    double field_of_view;
    matrix_t transform;
    matrix_t inverse_transform;
    double pixel_size;
    double half_width;
    double half_height;
} camera_t;

camera_t camera(const unsigned hsize, const unsigned vsize,
                const double field_of_view);

ray_t camera_ray_for_pixel(const camera_t *c, unsigned px, unsigned py);

void camera_set_transform(camera_t *c, matrix_t transform);

canvas_t *camera_render(const camera_t *c, const world_t *w);

#endif
