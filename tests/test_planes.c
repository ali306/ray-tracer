// test_planes.c

#include "../include/shapes.h"
#include <assert.h>
#include <stdio.h>

void test_planes(void)
{
    { // The normal of a plane is constant everywhere
        plane_t p  = plane();
        tuple_t n1 = shape_normal_at(&p, point(0, 0, 0), NULL);
        tuple_t n2 = shape_normal_at(&p, point(10, 0, -10), NULL);
        tuple_t n3 = shape_normal_at(&p, point(-5, 0, 150), NULL);

        tuple_t expected = vector(0, 1, 0);

        assert(tuple_equal(n1, expected));
        assert(tuple_equal(n2, expected));
        assert(tuple_equal(n3, expected));
    }

    { // Intersect with a ray parallel to the plane
        plane_t p = plane();
        ray_t r   = ray(point(0, 10, 0), vector(0, 0, 1));

        intersections_t xs = plane_intersect(&p, r);
        assert(xs.count == 0);
    }

    { // Intersect with a coplanar ray
        plane_t p = plane();
        ray_t r   = ray(point(0, 0, 0), vector(0, 0, 1));

        intersections_t xs = plane_intersect(&p, r);
        assert(xs.count == 0);
    }

    { // A ray intersecting a plane from above
        plane_t p = plane();
        ray_t r   = ray(point(0, 1, 0), vector(0, -1, 0));

        intersections_t xs = plane_intersect(&p, r);

        assert(xs.count == 1);
        assert(equal(xs.intersections[0].t, 1.0));
        assert(xs.intersections[0].object == &p);
    }

    { // A ray intersecting a plane from below
        plane_t p = plane();
        ray_t r   = ray(point(0, -1, 0), vector(0, 1, 0));

        intersections_t xs = plane_intersect(&p, r);

        assert(xs.count == 1);
        assert(equal(xs.intersections[0].t, 1.0));
        assert(xs.intersections[0].object == &p);
    }
}

int main(void)
{
    test_planes();
    return 0;
}
