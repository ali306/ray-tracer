// test_cones.c

#include "../include/shapes.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

void test_cones(void)
{
    { // Intersecting a cone with a ray (1)
        cone_t shape       = cone();
        tuple_t direction  = tuple_normalize(vector(0, 0, 1));
        ray_t r            = ray(point(0, 0, -5), direction);
        intersections_t xs = shape_intersect((shape_t *)&shape, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 5.0));
        assert(equal(xs.intersections[1].t, 5.0));
    }

    { // Intersecting a cone with a ray (2)
        cone_t shape       = cone();
        tuple_t direction  = tuple_normalize(vector(1, 1, 1));
        ray_t r            = ray(point(0, 0, -5), direction);
        intersections_t xs = shape_intersect((shape_t *)&shape, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 8.66025));
        assert(equal(xs.intersections[1].t, 8.66025));
    }

    { // Intersecting a cone with a ray (3)
        cone_t shape       = cone();
        tuple_t direction  = tuple_normalize(vector(-0.5, -1, 1));
        ray_t r            = ray(point(1, 1, -5), direction);
        intersections_t xs = shape_intersect((shape_t *)&shape, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.55006));
        assert(equal(xs.intersections[1].t, 49.44994));
    }

    { // Intersecting a cone with a ray parallel to one of its halves
        cone_t shape       = cone();
        tuple_t direction  = tuple_normalize(vector(0, 1, 1));
        ray_t r            = ray(point(0, 0, -1), direction);
        intersections_t xs = shape_intersect((shape_t *)&shape, r);

        assert(xs.count == 1);
        assert(equal(xs.intersections[0].t, 0.35355));
    }

    { // Intersecting a cone's end caps (1)
        cone_t shape       = cone();
        shape.minimum      = -0.5;
        shape.maximum      = 0.5;
        shape.closed       = true;
        tuple_t direction  = tuple_normalize(vector(0, 1, 0));
        ray_t r            = ray(point(0, 0, -5), direction);
        intersections_t xs = shape_intersect((shape_t *)&shape, r);

        assert(xs.count == 0);
    }

    { // Intersecting a cone's end caps (2)
        cone_t shape       = cone();
        shape.minimum      = -0.5;
        shape.maximum      = 0.5;
        shape.closed       = true;
        tuple_t direction  = tuple_normalize(vector(0, 1, 1));
        ray_t r            = ray(point(0, 0, -0.25), direction);
        intersections_t xs = shape_intersect((shape_t *)&shape, r);

        assert(xs.count == 2);
    }

    { // Intersecting a cone's end caps (3)
        cone_t shape       = cone();
        shape.minimum      = -0.5;
        shape.maximum      = 0.5;
        shape.closed       = true;
        tuple_t direction  = tuple_normalize(vector(0, 1, 0));
        ray_t r            = ray(point(0, 0, -0.25), direction);
        intersections_t xs = shape_intersect((shape_t *)&shape, r);

        assert(xs.count == 4);
    }

    printf("test_cones passed!\n");
}

int main(void)
{
    test_cones();
    return 0;
}
