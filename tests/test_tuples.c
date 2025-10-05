// test_tuples.c

#include "../include/tuples.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

void test_tuples(void)
{
    { // A tuple with w=1.0 is a point
        tuple_t a = {4.3, -4.2, 3.1, 1.0};
        assert(equal(a.w, 1.0));
    }

    { // A tuple with w=0.0 is a vector
        tuple_t a = {4.3, -4.2, 3.1, 0.0};
        assert(equal(a.w, 0.0));
    }

    { // tuple_point() creates tuples with w=1
        tuple_t p = point(4, -4, 3);
        assert(equal(p.x, 4) && equal(p.y, -4) && equal(p.z, 3) &&
               equal(p.w, 1));
    }

    { // tuple_vector() creates tuples with w=0
        tuple_t v = vector(4, -4, 3);
        assert(equal(v.x, 4) && equal(v.y, -4) && equal(v.z, 3) &&
               equal(v.w, 0));
    }

    { // Adding two tuples
        tuple_t a1 = {3, -2, 5, 1};
        tuple_t a2 = {-2, 3, 1, 0};
        tuple_t a3 = tuple_add(a1, a2);
        assert(equal(a3.x, 1) && equal(a3.y, 1) && equal(a3.z, 6) &&
               equal(a3.w, 1));
    }

    { // Subtracting two points
        tuple_t p1 = point(3, 2, 1);
        tuple_t p2 = point(5, 6, 7);
        tuple_t v  = tuple_subtract(p1, p2);
        assert(equal(v.x, -2) && equal(v.y, -4) && equal(v.z, -6) &&
               equal(v.w, 0));
    }

    { // Subtracting a vector from a point
        tuple_t p  = point(3, 2, 1);
        tuple_t v  = vector(5, 6, 7);
        tuple_t p2 = tuple_subtract(p, v);
        assert(equal(p2.x, -2) && equal(p2.y, -4) && equal(p2.z, -6) &&
               equal(p2.w, 1));
    }

    { // Subtracting two vectors
        tuple_t v1 = vector(3, 2, 1);
        tuple_t v2 = vector(5, 6, 7);
        tuple_t v3 = tuple_subtract(v1, v2);
        assert(equal(v3.x, -2) && equal(v3.y, -4) && equal(v3.z, -6) &&
               equal(v3.w, 0));
    }

    { // Negating a tuple
        tuple_t a = {1, -2, 3, -4};
        tuple_t b = tuple_negate(a);
        assert(equal(b.x, -1) && equal(b.y, 2) && equal(b.z, -3) &&
               equal(b.w, 4));
    }

    { // Multiplying a tuple by a scalar
        tuple_t a = {1, -2, 3, -4};
        tuple_t b = tuple_scale(a, 3.5);
        assert(equal(b.x, 3.5) && equal(b.y, -7.0) && equal(b.z, 10.5) &&
               equal(b.w, -14.0));
    }

    { // Multiplying a tuple by a fraction
        tuple_t a = {1, -2, 3, -4};
        tuple_t b = tuple_scale(a, 0.5);
        assert(equal(b.x, 0.5) && equal(b.y, -1.0) && equal(b.z, 1.5) &&
               equal(b.w, -2.0));
    }

    { // Dividing a tuple by a scalar
        tuple_t a = {1, -2, 3, -4};
        tuple_t b = tuple_scale(a, 0.5);
        assert(equal(b.x, 0.5) && equal(b.y, -1.0) && equal(b.z, 1.5) &&
               equal(b.w, -2.0));
    }

    { // Computing the magnitude of vector(1, 0, 0)
        tuple_t v = vector(1, 0, 0);
        assert(equal(tuple_magnitude(v), 1.0));
    }

    { // Computing the magnitude of vector(0, 1, 0)
        tuple_t v = vector(0, 1, 0);
        assert(equal(tuple_magnitude(v), 1.0));
    }

    { // Computing the magnitude of vector(0, 0, 1)
        tuple_t v = vector(0, 0, 1);
        assert(equal(tuple_magnitude(v), 1.0));
    }

    { // Computing the magnitude of vector(1, 2, 3)
        tuple_t v = vector(1, 2, 3);
        assert(equal(tuple_magnitude(v), sqrt(14.0)));
    }

    { // Computing the magnitude of vector(-1, -2, -3)
        tuple_t v = vector(-1, -2, -3);
        assert(equal(tuple_magnitude(v), sqrt(14.0)));
    }

    { // Normalizing vector(4, 0, 0) gives (1, 0, 0)
        tuple_t v    = vector(4, 0, 0);
        tuple_t norm = tuple_normalize(v);
        assert(equal(norm.x, 1) && equal(norm.y, 0) && equal(norm.z, 0));
    }

    { // Normalizing vector(1, 2, 3)
        tuple_t v    = vector(1, 2, 3);
        tuple_t norm = tuple_normalize(v);
        assert(equal(norm.x, 1 / sqrt(14.0)) && equal(norm.y, 2 / sqrt(14.0)) &&
               equal(norm.z, 3 / sqrt(14.0)));
    }

    { // The magnitude of a normalized vector
        tuple_t v    = vector(1, 2, 3);
        tuple_t norm = tuple_normalize(v);
        assert(equal(tuple_magnitude(norm), 1.0));
    }

    { // The dot product of two tuples
        tuple_t a = vector(1, 2, 3);
        tuple_t b = vector(2, 3, 4);
        assert(equal(tuple_dot(a, b), 20.0));
    }

    { // The cross product of two vectors
        tuple_t a = vector(1, 2, 3);
        tuple_t b = vector(2, 3, 4);
        tuple_t c = tuple_cross(a, b);
        assert(equal(c.x, -1.0) && equal(c.y, 2.0) && equal(c.z, -1.0));
    }

    { // Colors are (red, green, blue) tuples
        tuple_t c = color(0.9, 0.6, 0.75);
        assert(equal(c.x, 0.9) && equal(c.y, 0.6) && equal(c.z, 0.75));
    }

    { // Adding colors
        tuple_t c1 = color(0.9, 0.6, 0.75);
        tuple_t c2 = color(0.7, 0.1, 0.25);
        tuple_t c3 = tuple_add(c1, c2);
        assert(equal(c3.x, 1.6) && equal(c3.y, 0.7) && equal(c3.z, 1.0));
    }

    { // Subtracting colors
        tuple_t c1 = color(0.9, 0.6, 0.75);
        tuple_t c2 = color(0.7, 0.1, 0.25);
        tuple_t c3 = tuple_subtract(c1, c2);
        assert(equal(c3.x, 0.2) && equal(c3.y, 0.5) && equal(c3.z, 0.5));
    }

    { // Multiplying a color by a scalar
        tuple_t c  = color(0.2, 0.3, 0.4);
        tuple_t c2 = tuple_scale(c, 2);
        assert(equal(c2.x, 0.4) && equal(c2.y, 0.6) && equal(c2.z, 0.8));
    }

    { // Multiplying colors
        tuple_t c1 = color(1, 0.2, 0.4);
        tuple_t c2 = color(0.9, 1, 0.1);
        tuple_t c3 = tuple_hadamard(c1, c2);
        assert(equal(c3.x, 0.9) && equal(c3.y, 0.2) && equal(c3.z, 0.04));
    }

    { // Reflecting a vector approaching at 45°
        tuple_t v = vector(1, -1, 0);
        tuple_t n = vector(0, 1, 0);
        tuple_t r = tuple_reflect(v, n);
        assert(equal(r.x, 1) && equal(r.y, 1) && equal(r.z, 0));
    }

    { // Reflecting a vector off a slanted surface
        tuple_t v = vector(0, -1, 0);
        tuple_t n = vector(sqrt(2.0) / 2.0, sqrt(2.0) / 2.0, 0);
        tuple_t r = tuple_reflect(v, n);
        assert(equal(r.x, 1) && equal(r.y, 0) && equal(r.z, 0));
    }
}

int main(void)
{
    test_tuples();
    return 0;
}
