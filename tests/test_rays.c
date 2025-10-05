// test_rays.c

#include "../include/rays.h"
#include <assert.h>
#include <stdio.h>

void test_rays(void)
{
    { // Creating and querying a ray
        tuple_t origin    = point(1, 2, 3);
        tuple_t direction = vector(4, 5, 6);
        ray_t r           = ray(origin, direction);
        assert(tuple_equal(r.origin, origin));
        assert(tuple_equal(r.direction, direction));
    }

    { // Computing a point from a distance
        ray_t r = ray(point(2, 3, 4), vector(1, 0, 0));

        assert(tuple_equal(ray_position(r, 0), point(2, 3, 4)));
        assert(tuple_equal(ray_position(r, 1), point(3, 3, 4)));
        assert(tuple_equal(ray_position(r, -1), point(1, 3, 4)));
        assert(tuple_equal(ray_position(r, 2.5), point(4.5, 3, 4)));
    }

    { // Translating a ray
        ray_t r    = ray(point(1, 2, 3), vector(0, 1, 0));
        matrix_t m = transform_translation(3, 4, 5);

        ray_t r2 = ray_transform(r, m);

        assert(tuple_equal(r2.origin, point(4, 6, 8)));
        assert(tuple_equal(r2.direction, vector(0, 1, 0)));
    }

    { // Scaling a ray
        ray_t r    = ray(point(1, 2, 3), vector(0, 1, 0));
        matrix_t m = transform_scaling(2, 3, 4);

        ray_t r2 = ray_transform(r, m);

        assert(tuple_equal(r2.origin, point(2, 6, 12)));
        assert(tuple_equal(r2.direction, vector(0, 3, 0)));
    }
}

int main(void)
{
    test_rays();
    return 0;
}
