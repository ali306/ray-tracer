// test_cylinders.c

#include "../include/shapes.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

void test_cylinders(void)
{
    { // The default minimum and maximum for a cylinder
        cylinder_t cyl = cylinder();

        assert(cyl.minimum < -1e308);
        assert(cyl.maximum > 1e308);
    }

    { // The default closed value for a cylinder
        cylinder_t cyl = cylinder();

        assert(cyl.closed == false);
    }

    { // A ray misses a cylinder (ray on surface, parallel to y-axis)
        cylinder_t cyl     = cylinder();
        tuple_t direction  = tuple_normalize(vector(0, 1, 0));
        ray_t r            = ray(point(1, 0, 0), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 0);
    }

    { // A ray misses a cylinder (ray inside, parallel to y-axis)
        cylinder_t cyl     = cylinder();
        tuple_t direction  = tuple_normalize(vector(0, 1, 0));
        ray_t r            = ray(point(0, 0, 0), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 0);
    }

    { // A ray misses a cylinder (ray outside, askew)
        cylinder_t cyl     = cylinder();
        tuple_t direction  = tuple_normalize(vector(1, 1, 1));
        ray_t r            = ray(point(0, 0, -5), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 0);
    }

    { // A ray strikes a cylinder (tangent, single point)
        cylinder_t cyl     = cylinder();
        tuple_t direction  = tuple_normalize(vector(0, 0, 1));
        ray_t r            = ray(point(1, 0, -5), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 5.0));
        assert(equal(xs.intersections[1].t, 5.0));
    }

    { // A ray strikes a cylinder (perpendicular through middle)
        cylinder_t cyl     = cylinder();
        tuple_t direction  = tuple_normalize(vector(0, 0, 1));
        ray_t r            = ray(point(0, 0, -5), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
    }

    { // A ray strikes a cylinder (at an angle)
        cylinder_t cyl     = cylinder();
        tuple_t direction  = tuple_normalize(vector(0.1, 1, 1));
        ray_t r            = ray(point(0.5, 0, -5), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 6.80798));
        assert(equal(xs.intersections[1].t, 7.08872));
    }

    { // Normal vector on a cylinder at +x
        cylinder_t cyl = cylinder();
        tuple_t n      = shape_normal_at((shape_t *)&cyl, point(1, 0, 0), NULL);

        assert(tuple_equal(n, vector(1, 0, 0)));
    }

    { // Normal vector on a cylinder at -z
        cylinder_t cyl = cylinder();
        tuple_t n      = shape_normal_at((shape_t *)&cyl, point(0, 5, -1), NULL);

        assert(tuple_equal(n, vector(0, 0, -1)));
    }

    { // Normal vector on a cylinder at +z
        cylinder_t cyl = cylinder();
        tuple_t n      = shape_normal_at((shape_t *)&cyl, point(0, -2, 1), NULL);

        assert(tuple_equal(n, vector(0, 0, 1)));
    }

    { // Normal vector on a cylinder at -x
        cylinder_t cyl = cylinder();
        tuple_t n      = shape_normal_at((shape_t *)&cyl, point(-1, 1, 0), NULL);

        assert(tuple_equal(n, vector(-1, 0, 0)));
    }

    { // Intersecting the caps of a closed cylinder (1)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t direction  = tuple_normalize(vector(0, -1, 0));
        ray_t r            = ray(point(0, 3, 0), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 2);
    }

    { // Intersecting the caps of a closed cylinder (2)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t direction  = tuple_normalize(vector(0, -1, 2));
        ray_t r            = ray(point(0, 3, -2), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 2);
    }

    { // Intersecting the caps of a closed cylinder (3)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t direction  = tuple_normalize(vector(0, -1, 1));
        ray_t r            = ray(point(0, 4, -2), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 2);
    }

    { // Intersecting the caps of a closed cylinder (4)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t direction  = tuple_normalize(vector(0, 1, 2));
        ray_t r            = ray(point(0, 0, -2), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 2);
    }

    { // Intersecting the caps of a closed cylinder (5)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t direction  = tuple_normalize(vector(0, 1, 1));
        ray_t r            = ray(point(0, -1, -2), direction);
        intersections_t xs = shape_intersect((shape_t *)&cyl, r);

        assert(xs.count == 2);
    }

    { // Normal vector on cylinder's end caps (bottom center)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t n = shape_normal_at((shape_t *)&cyl, point(0, 1, 0), NULL);

        assert(tuple_equal(n, vector(0, -1, 0)));
    }

    { // Normal vector on cylinder's end caps (bottom off-center x)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t n = shape_normal_at((shape_t *)&cyl, point(0.5, 1, 0), NULL);

        assert(tuple_equal(n, vector(0, -1, 0)));
    }

    { // Normal vector on cylinder's end caps (bottom off-center z)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t n = shape_normal_at((shape_t *)&cyl, point(0, 1, 0.5), NULL);

        assert(tuple_equal(n, vector(0, -1, 0)));
    }

    { // Normal vector on cylinder's end caps (top center)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t n = shape_normal_at((shape_t *)&cyl, point(0, 2, 0), NULL);

        assert(tuple_equal(n, vector(0, 1, 0)));
    }

    { // Normal vector on cylinder's end caps (top off-center x)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t n = shape_normal_at((shape_t *)&cyl, point(0.5, 2, 0), NULL);

        assert(tuple_equal(n, vector(0, 1, 0)));
    }

    { // Normal vector on cylinder's end caps (top off-center z)
        cylinder_t cyl = cylinder();
        cyl.minimum    = 1.0;
        cyl.maximum    = 2.0;
        cyl.closed     = true;
        tuple_t n = shape_normal_at((shape_t *)&cyl, point(0, 2, 0.5), NULL);

        assert(tuple_equal(n, vector(0, 1, 0)));
    }

    printf("test_cylinders passed!\n");
}

int main(void)
{
    test_cylinders();
    return 0;
}
