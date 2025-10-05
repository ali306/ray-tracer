// test_cubes.c

#include "../include/shapes.h"
#include <assert.h>
#include <stdio.h>

void test_cubes(void)
{
    { // A ray intersects a cube from the +x direction
        cube_t c           = cube();
        ray_t r            = ray(point(5, 0.5, 0), vector(-1, 0, 0));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
    }

    { // A ray intersects a cube from the -x direction
        cube_t c           = cube();
        ray_t r            = ray(point(-5, 0.5, 0), vector(1, 0, 0));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
    }

    { // A ray intersects a cube from the +y direction
        cube_t c           = cube();
        ray_t r            = ray(point(0.5, 5, 0), vector(0, -1, 0));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
    }

    { // A ray intersects a cube from the -y direction
        cube_t c           = cube();
        ray_t r            = ray(point(0.5, -5, 0), vector(0, 1, 0));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
    }

    { // A ray intersects a cube from the +z direction
        cube_t c           = cube();
        ray_t r            = ray(point(0.5, 0, 5), vector(0, 0, -1));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
    }

    { // A ray intersects a cube from the -z direction
        cube_t c           = cube();
        ray_t r            = ray(point(0.5, 0, -5), vector(0, 0, 1));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
    }

    { // A ray intersects a cube from the inside
        cube_t c           = cube();
        ray_t r            = ray(point(0, 0.5, 0), vector(0, 0, 1));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, -1.0));
        assert(equal(xs.intersections[1].t, 1.0));
    }

    { // A ray misses a cube (parallel to x faces)
        cube_t c = cube();
        ray_t r  = ray(point(-2, 0, 0), vector(0.2673, 0.5345, 0.8018));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 0);
    }

    { // A ray misses a cube (parallel to y faces)
        cube_t c = cube();
        ray_t r  = ray(point(0, -2, 0), vector(0.8018, 0.2673, 0.5345));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 0);
    }

    { // A ray misses a cube (parallel to z faces)
        cube_t c = cube();
        ray_t r  = ray(point(0, 0, -2), vector(0.5345, 0.8018, 0.2673));
        intersections_t xs = shape_intersect(&c, r);

        assert(xs.count == 0);
    }

    { // The normal on the surface of a cube (positive x)
        cube_t c       = cube();
        tuple_t p      = point(1, 0.5, -0.8);
        tuple_t normal = shape_normal_at(&c, p, NULL);
        assert(tuple_equal(normal, vector(1, 0, 0)));
    }

    { // The normal on the surface of a cube (negative x)
        cube_t c       = cube();
        tuple_t p      = point(-1, -0.2, 0.9);
        tuple_t normal = shape_normal_at(&c, p, NULL);
        assert(tuple_equal(normal, vector(-1, 0, 0)));
    }

    { // The normal on the surface of a cube (positive y)
        cube_t c       = cube();
        tuple_t p      = point(-0.4, 1, -0.1);
        tuple_t normal = shape_normal_at(&c, p, NULL);
        assert(tuple_equal(normal, vector(0, 1, 0)));
    }

    { // The normal on the surface of a cube (negative y)
        cube_t c       = cube();
        tuple_t p      = point(0.3, -1, -0.7);
        tuple_t normal = shape_normal_at(&c, p, NULL);
        assert(tuple_equal(normal, vector(0, -1, 0)));
    }

    { // The normal on the surface of a cube (positive z)
        cube_t c       = cube();
        tuple_t p      = point(-0.6, 0.3, 1);
        tuple_t normal = shape_normal_at(&c, p, NULL);
        assert(tuple_equal(normal, vector(0, 0, 1)));
    }

    { // The normal on the surface of a cube (negative z)
        cube_t c       = cube();
        tuple_t p      = point(0.4, 0.4, -1);
        tuple_t normal = shape_normal_at(&c, p, NULL);
        assert(tuple_equal(normal, vector(0, 0, -1)));
    }

    { // The normal on the surface of a cube (at a corner - positive x
      // dominates)
        cube_t c       = cube();
        tuple_t p      = point(1, 1, 1);
        tuple_t normal = shape_normal_at(&c, p, NULL);
        assert(tuple_equal(normal, vector(1, 0, 0)));
    }

    { // The normal on the surface of a cube (at a corner - negative x
      // dominates)
        cube_t c       = cube();
        tuple_t p      = point(-1, -1, -1);
        tuple_t normal = shape_normal_at(&c, p, NULL);
        assert(tuple_equal(normal, vector(-1, 0, 0)));
    }
}

int main(void)
{
    test_cubes();
    return 0;
}
