// test_triangles.c

#include "../include/shapes.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

void test_triangles(void)
{
    { // Constructing a triangle
        tuple_t p1   = point(0, 1, 0);
        tuple_t p2   = point(-1, 0, 0);
        tuple_t p3   = point(1, 0, 0);
        triangle_t t = triangle(p1, p2, p3);

        assert(tuple_equal(t.p1, p1));
        assert(tuple_equal(t.p2, p2));
        assert(tuple_equal(t.p3, p3));
        assert(tuple_equal(t.e1, vector(-1, -1, 0)));
        assert(tuple_equal(t.e2, vector(1, -1, 0)));
        assert(tuple_equal(t.normal, vector(0, 0, -1)));
    }

    { // Finding the normal on a triangle
        triangle_t t =
            triangle(point(0, 1, 0), point(-1, 0, 0), point(1, 0, 0));
        tuple_t n1 = shape_normal_at((shape_t *)&t, point(0, 0.5, 0), NULL);
        tuple_t n2 = shape_normal_at((shape_t *)&t, point(-0.5, 0.75, 0), NULL);
        tuple_t n3 = shape_normal_at((shape_t *)&t, point(0.5, 0.25, 0), NULL);

        assert(tuple_equal(n1, t.normal));
        assert(tuple_equal(n2, t.normal));
        assert(tuple_equal(n3, t.normal));
    }

    { // Intersecting a ray parallel to the triangle
        triangle_t t =
            triangle(point(0, 1, 0), point(-1, 0, 0), point(1, 0, 0));
        ray_t r            = ray(point(0, -1, -2), vector(0, 1, 0));
        intersections_t xs = triangle_intersect(&t, r);

        assert(xs.count == 0);
    }

    { // A ray misses the p1-p3 edge
        triangle_t t =
            triangle(point(0, 1, 0), point(-1, 0, 0), point(1, 0, 0));
        ray_t r            = ray(point(1, 1, -2), vector(0, 0, 1));
        intersections_t xs = triangle_intersect(&t, r);

        assert(xs.count == 0);
    }

    { // A ray misses the p1-p2 edge
        triangle_t t =
            triangle(point(0, 1, 0), point(-1, 0, 0), point(1, 0, 0));
        ray_t r            = ray(point(-1, 1, -2), vector(0, 0, 1));
        intersections_t xs = triangle_intersect(&t, r);

        assert(xs.count == 0);
    }

    { // A ray misses the p2-p3 edge
        triangle_t t =
            triangle(point(0, 1, 0), point(-1, 0, 0), point(1, 0, 0));
        ray_t r            = ray(point(0, -1, -2), vector(0, 0, 1));
        intersections_t xs = triangle_intersect(&t, r);

        assert(xs.count == 0);
    }

    { // A ray strikes a triangle
        triangle_t t =
            triangle(point(0, 1, 0), point(-1, 0, 0), point(1, 0, 0));
        ray_t r            = ray(point(0, 0.5, -2), vector(0, 0, 1));
        intersections_t xs = triangle_intersect(&t, r);

        assert(xs.count == 1);
        assert(equal(xs.intersections[0].t, 2.0));
    }

    { // Constructing a smooth triangle
        tuple_t p1            = point(0, 1, 0);
        tuple_t p2            = point(-1, 0, 0);
        tuple_t p3            = point(1, 0, 0);
        tuple_t n1            = vector(0, 1, 0);
        tuple_t n2            = vector(-1, 0, 0);
        tuple_t n3            = vector(1, 0, 0);
        smooth_triangle_t tri = smooth_triangle(p1, p2, p3, n1, n2, n3);

        assert(tuple_equal(tri.p1, p1));
        assert(tuple_equal(tri.p2, p2));
        assert(tuple_equal(tri.p3, p3));
        assert(tuple_equal(tri.n1, n1));
        assert(tuple_equal(tri.n2, n2));
        assert(tuple_equal(tri.n3, n3));
    }

    { // An intersection with a smooth triangle stores u/v
        tuple_t p1            = point(0, 1, 0);
        tuple_t p2            = point(-1, 0, 0);
        tuple_t p3            = point(1, 0, 0);
        tuple_t n1            = vector(0, 1, 0);
        tuple_t n2            = vector(-1, 0, 0);
        tuple_t n3            = vector(1, 0, 0);
        smooth_triangle_t tri = smooth_triangle(p1, p2, p3, n1, n2, n3);

        ray_t r            = ray(point(-0.2, 0.3, -2), vector(0, 0, 1));
        intersections_t xs = smooth_triangle_intersect(&tri, r);

        assert(equal(xs.intersections[0].u, 0.45));
        assert(equal(xs.intersections[0].v, 0.25));
    }

    { // A smooth triangle uses u/v to interpolate the normal
        tuple_t p1            = point(0, 1, 0);
        tuple_t p2            = point(-1, 0, 0);
        tuple_t p3            = point(1, 0, 0);
        tuple_t n1            = vector(0, 1, 0);
        tuple_t n2            = vector(-1, 0, 0);
        tuple_t n3            = vector(1, 0, 0);
        smooth_triangle_t tri = smooth_triangle(p1, p2, p3, n1, n2, n3);

        intersection_t i = intersection_with_uv(1, &tri, 0.45, 0.25);
        tuple_t n        = normal_at(&tri, point(0, 0, 0), &i);

        assert(tuple_equal(n, vector(-0.5547, 0.83205, 0)));
    }

    { // Preparing the normal on a smooth triangle
        tuple_t p1            = point(0, 1, 0);
        tuple_t p2            = point(-1, 0, 0);
        tuple_t p3            = point(1, 0, 0);
        tuple_t n1            = vector(0, 1, 0);
        tuple_t n2            = vector(-1, 0, 0);
        tuple_t n3            = vector(1, 0, 0);
        smooth_triangle_t tri = smooth_triangle(p1, p2, p3, n1, n2, n3);

        intersection_t i     = intersection_with_uv(1, &tri, 0.45, 0.25);
        ray_t r              = ray(point(-0.2, 0.3, -2), vector(0, 0, 1));
        intersections_t xs   = intersections(1, i);
        computations_t comps = intersections_prepare_computations(&i, &r, &xs);

        assert(tuple_equal(comps.normalv, vector(-0.5547, 0.83205, 0)));
    }
}

int main(void)
{
    test_triangles();
    return 0;
}