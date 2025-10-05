#include "../include/camera.h"
#include <math.h>

#include <omp.h>
#include <stdio.h>

camera_t camera(const unsigned hsize, const unsigned vsize,
                const double field_of_view)
{
    camera_t c = {hsize, vsize, field_of_view, IDENTITY, IDENTITY, 1, 1, 1};
    double half_view = tan(c.field_of_view / 2);
    double aspect    = (double)c.hsize / (double)c.vsize;

    if (aspect >= 1)
    {
        c.half_width  = half_view;
        c.half_height = half_view / aspect;
    }
    else
    {
        c.half_width  = half_view * aspect;
        c.half_height = half_view;
    }

    c.pixel_size = (c.half_width * 2.0) / c.hsize;

    return c;
}

ray_t camera_ray_for_pixel(const camera_t *c, unsigned px, unsigned py)
{
    if (c == NULL)
    {
        return ray(point(0, 0, 0), vector(0, 0, 1));
    }

    double xoffset = (px + 0.5) * c->pixel_size;
    double yoffset = (py + 0.5) * c->pixel_size;

    double world_x = c->half_width - xoffset;
    double world_y = c->half_height - yoffset;

    tuple_t origin    = matrix_tmul(c->inverse_transform, point(0, 0, 0));
    tuple_t direction = tuple_normalize(tuple_subtract(
        matrix_tmul(c->inverse_transform, point(world_x, world_y, -1)),
        origin));

    return ray(origin, direction);
}

void camera_set_transform(camera_t *c, matrix_t transform)
{
    if (c == NULL)
    {
        return;
    }

    c->transform         = transform;
    c->inverse_transform = matrix_inverse(transform);
}

canvas_t *camera_render(const camera_t *c, const world_t *w)
{
    if (!c || !w)
    {
        printf("Invalid parameters for camera_render\n");
        return NULL;
    }

    canvas_t *image = canvas(c->hsize, c->vsize);
    if (!image)
    {
        printf("Failed to create canvas for rendering\n");
        return NULL;
    }

    printf("Rendering %dx%d image...\n", c->hsize, c->vsize);

#pragma omp parallel for schedule(dynamic, 64) collapse(2)
    for (unsigned y = 0; y < c->vsize; y++)
    {
        for (unsigned x = 0; x < c->hsize; x++)
        {
            ray_t ray     = camera_ray_for_pixel(c, x, y);
            tuple_t color = world_color_at(w, &ray, MAX_RECURSION);
            canvas_write_pixel(image, x, y, color);
        }
    }

    printf("Rendering complete!\n");
    return image;
}
