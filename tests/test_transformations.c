// test_transformations.c

#include "../include/transformations.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

void test_transformations(void)
{
    { // Multiplying by a translation matrix
        matrix_t transform = transform_translation(5, -3, 2);
        tuple_t p          = point(-3, 4, 5);
        assert(tuple_equal(matrix_tmul(transform, p), point(2, 1, 7)));
    }

    { // Multiplying by the inverse of a translation matrix
        matrix_t transform = transform_translation(5, -3, 2);
        matrix_t inv       = matrix_inverse(transform);
        tuple_t p          = point(-3, 4, 5);
        assert(tuple_equal(matrix_tmul(inv, p), point(-8, 7, 3)));
    }

    { // Translation does not affect vectors
        matrix_t transform = transform_translation(5, -3, 2);
        tuple_t v          = vector(-3, 4, 5);
        assert(tuple_equal(matrix_tmul(transform, v), v));
    }

    { // A scaling matrix applied to a point
        matrix_t scaling = transform_scaling(2, 3, 4);
        tuple_t p        = point(-4, 6, 8);
        assert(tuple_equal(matrix_tmul(scaling, p), point(-8, 18, 32)));
    }

    { // A scaling matrix applied to a vector
        matrix_t scaling = transform_scaling(2, 3, 4);
        tuple_t v        = vector(-4, 6, 8);
        assert(tuple_equal(matrix_tmul(scaling, v), vector(-8, 18, 32)));
    }

    { // Multiplying by the inverse of a scaling matrix
        matrix_t scaling = transform_scaling(2, 3, 4);
        matrix_t inv     = matrix_inverse(scaling);
        tuple_t v        = vector(-4, 6, 8);
        assert(tuple_equal(matrix_tmul(inv, v), vector(-2, 2, 2)));
    }

    { // Reflection is scaling by a negative value
        matrix_t reflection = transform_scaling(-1, 1, 1);
        tuple_t p           = point(2, 3, 4);
        assert(tuple_equal(matrix_tmul(reflection, p), point(-2, 3, 4)));
    }

    { // Rotating a point around the x axis
        tuple_t p             = point(0, 1, 0);
        tuple_t expected_half = point(0, sqrt(2.0) / 2.0, sqrt(2.0) / 2.0);
        tuple_t expected_full = point(0, 0, 1);

        double angle_half = M_PI_4;
        double angle_full = M_PI_2;

        matrix_t half_quarter = transform_rotation_x(angle_half);
        matrix_t full_quarter = transform_rotation_x(angle_full);

        assert(tuple_equal(matrix_tmul(half_quarter, p), expected_half));
        assert(tuple_equal(matrix_tmul(full_quarter, p), expected_full));
    }

    { // The inverse of an x-rotation rotates in the opposite direction
        tuple_t p        = point(0, 1, 0);
        tuple_t expected = point(0, sqrt(2.0) / 2.0, -sqrt(2.0) / 2.0);

        double angle          = M_PI_4;
        matrix_t half_quarter = transform_rotation_x(angle);
        matrix_t inv          = matrix_inverse(half_quarter);

        assert(tuple_equal(matrix_tmul(inv, p), expected));
    }

    { // Rotating a point around the y axis
        tuple_t p             = point(0, 0, 1);
        tuple_t expected_half = point(sqrt(2.0) / 2.0, 0, sqrt(2.0) / 2.0);
        tuple_t expected_full = point(1, 0, 0);

        double angle_half = M_PI_4;
        double angle_full = M_PI_2;

        matrix_t half_quarter = transform_rotation_y(angle_half);
        matrix_t full_quarter = transform_rotation_y(angle_full);

        assert(tuple_equal(matrix_tmul(half_quarter, p), expected_half));
        assert(tuple_equal(matrix_tmul(full_quarter, p), expected_full));
    }

    { // Rotating a point around the z axis
        tuple_t p             = point(0, 1, 0);
        tuple_t expected_half = point(-sqrt(2.0) / 2.0, sqrt(2.0) / 2.0, 0);
        tuple_t expected_full = point(-1, 0, 0);

        double angle_half = M_PI_4;
        double angle_full = M_PI_2;

        matrix_t half_quarter = transform_rotation_z(angle_half);
        matrix_t full_quarter = transform_rotation_z(angle_full);

        assert(tuple_equal(matrix_tmul(half_quarter, p), expected_half));
        assert(tuple_equal(matrix_tmul(full_quarter, p), expected_full));
    }

    { // A shearing transformation moves x in proportion to y
        matrix_t transform = transform_shearing(1, 0, 0, 0, 0, 0);
        tuple_t p          = point(2, 3, 4);
        tuple_t result     = matrix_tmul(transform, p);

        assert(equal(result.x, 5));
        assert(equal(result.y, 3));
        assert(equal(result.z, 4));
    }

    { // A shearing transformation moves x in proportion to z
        matrix_t transform = transform_shearing(0, 1, 0, 0, 0, 0);
        tuple_t p          = point(2, 3, 4);
        tuple_t result     = matrix_tmul(transform, p);

        assert(equal(result.x, 6));
        assert(equal(result.y, 3));
        assert(equal(result.z, 4));
    }

    { // A shearing transformation moves y in proportion to x
        matrix_t transform = transform_shearing(0, 0, 1, 0, 0, 0);
        tuple_t p          = point(2, 3, 4);
        tuple_t result     = matrix_tmul(transform, p);

        assert(equal(result.x, 2));
        assert(equal(result.y, 5));
        assert(equal(result.z, 4));
    }

    { // A shearing transformation moves y in proportion to z
        matrix_t transform = transform_shearing(0, 0, 0, 1, 0, 0);
        tuple_t p          = point(2, 3, 4);
        tuple_t result     = matrix_tmul(transform, p);

        assert(equal(result.x, 2));
        assert(equal(result.y, 7));
        assert(equal(result.z, 4));
    }

    { // A shearing transformation moves z in proportion to x
        matrix_t transform = transform_shearing(0, 0, 0, 0, 1, 0);
        tuple_t p          = point(2, 3, 4);
        tuple_t result     = matrix_tmul(transform, p);

        assert(equal(result.x, 2));
        assert(equal(result.y, 3));
        assert(equal(result.z, 6));
    }

    { // A shearing transformation moves z in proportion to y
        matrix_t transform = transform_shearing(0, 0, 0, 0, 0, 1);
        tuple_t p          = point(2, 3, 4);
        tuple_t result     = matrix_tmul(transform, p);

        assert(equal(result.x, 2));
        assert(equal(result.y, 3));
        assert(equal(result.z, 7));
    }

    { // Individual transformations are applied in sequence
        tuple_t p  = point(1, 0, 1);
        matrix_t A = transform_rotation_x(M_PI_2);
        matrix_t B = transform_scaling(5, 5, 5);
        matrix_t C = transform_translation(10, 5, 7);

        tuple_t p2 = matrix_tmul(A, p);
        assert(equal(p2.x, 1));
        assert(equal(p2.y, -1));
        assert(equal(p2.z, 0));

        tuple_t p3 = matrix_tmul(B, p2);
        assert(equal(p3.x, 5));
        assert(equal(p3.y, -5));
        assert(equal(p3.z, 0));

        tuple_t p4 = matrix_tmul(C, p3);
        assert(equal(p4.x, 15));
        assert(equal(p4.y, 0));
        assert(equal(p4.z, 7));
    }

    { // Chained transformations are applied in reverse order
        tuple_t p  = point(1, 0, 1);
        matrix_t A = transform_rotation_x(M_PI_2);
        matrix_t B = transform_scaling(5, 5, 5);
        matrix_t C = transform_translation(10, 5, 7);

        matrix_t T = matrix_mul(C, matrix_mul(B, A));

        tuple_t result = matrix_tmul(T, p);
        assert(equal(result.x, 15));
        assert(equal(result.y, 0));
        assert(equal(result.z, 7));
    }

    { // The view transformation for the default orientation
        tuple_t from = point(0, 0, 0);
        tuple_t to   = point(0, 0, -1);
        tuple_t up   = vector(0, 1, 0);

        assert(matrix_equal(transform_view(from, to, up), IDENTITY));
    }

    { // The view transformation looking in positive z direction
        tuple_t from = point(0, 0, 0);
        tuple_t to   = point(0, 0, 1);
        tuple_t up   = vector(0, 1, 0);

        assert(matrix_equal(transform_view(from, to, up),
                            transform_scaling(-1, 1, -1)));
    }

    { // The view transformation moves the world
        tuple_t from = point(0, 0, 8);
        tuple_t to   = point(0, 0, 0);
        tuple_t up   = vector(0, 1, 0);

        assert(matrix_equal(transform_view(from, to, up),
                            transform_translation(0, 0, -8)));
    }

    { // An arbitrary view transformation
        tuple_t from = point(1, 3, 2);
        tuple_t to   = point(4, -2, 8);
        tuple_t up   = vector(1, 1, 0);

        matrix_t expected = {-0.50709, 0.50709, 0.67612,  -2.36643,
                             0.76772,  0.60609, 0.12122,  -2.82843,
                             -0.35857, 0.59761, -0.71714, 0.00000,
                             0.00000,  0.00000, 0.00000,  1.00000};

        assert(matrix_equal(transform_view(from, to, up), expected));
    }
}

int main(void)
{
    test_transformations();
    return 0;
}
