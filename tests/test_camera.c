// test_camera.c

#include "../include/camera.h"
#include <assert.h>
#include <math.h>

void test_camera(void)
{
    { // Constructing a camera
        unsigned hsize = 160, vsize = 120;
        double field_of_view = M_PI_2;
        camera_t c           = camera(hsize, vsize, field_of_view);

        assert(c.hsize == hsize);
        assert(c.vsize == vsize);
        assert(equal(c.field_of_view, field_of_view));
        assert(matrix_equal(c.transform, IDENTITY));
    }

    { // The pixel size for a horizontal canvas
        camera_t c = camera(200, 125, M_PI_2);
        printf("%lf\n", c.pixel_size);
        assert(equal(c.pixel_size, 0.01));
    }

    { // The pixel size for a vertical canvas
        camera_t c = camera(125, 200, M_PI_2);
        printf("%lf\n", c.pixel_size);
        assert(equal(c.pixel_size, 0.01));
    }

    { // Constructing a ray through the center of the canvas
        camera_t c = camera(201, 101, M_PI_2);
        ray_t r    = camera_ray_for_pixel(&c, 100, 50);

        assert(tuple_equal(r.origin, point(0, 0, 0)));
        assert(tuple_equal(r.direction, vector(0, 0, -1)));
    }

    { // Constructing a ray through a corner of the canvas
        camera_t c = camera(201, 101, M_PI_2);
        ray_t r    = camera_ray_for_pixel(&c, 0, 0);

        assert(tuple_equal(r.origin, point(0, 0, 0)));
        assert(tuple_equal(r.direction, vector(0.66519, 0.33259, -0.66851)));
    }

    { // Constructing a ray when the camera is transformed
        camera_t c = camera(201, 101, M_PI_2);
        camera_set_transform(&c, matrix_mul(transform_rotation_y(M_PI_4),
                                            transform_translation(0, -2, 5)));
        ray_t r = camera_ray_for_pixel(&c, 100, 50);

        assert(tuple_equal(r.origin, point(0, 2, -5)));
        assert(tuple_equal(r.direction,
                           vector(sqrt(2.0) / 2.0, 0, -sqrt(2.0) / 2.0)));
    }

    { // Rendering a world with a camera
        world_t w    = world_default();
        camera_t c   = camera(11, 11, M_PI_2);
        tuple_t from = point(0, 0, -5);
        tuple_t to   = point(0, 0, 0);
        tuple_t up   = vector(0, 1, 0);
        camera_set_transform(&c, transform_view(from, to, up));
        canvas_t *image = camera_render(&c, &w);

        assert(tuple_equal(canvas_pixel_at(image, 5, 5),
                           color(0.38066, 0.47583, 0.2855)));

        canvas_free(image);
        world_free(&w);
    }
}

int main(void)
{
    test_camera();
    return 0;
}
