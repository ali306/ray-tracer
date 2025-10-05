// test_matrices.c

#include "../include/matrices.h"
#include <assert.h>
#include <stdio.h>

void test_matrices(void)
{
    { // Constructing and inspecting a 4x4 matrix
        matrix_t M;

        double values[] = {1, 2,  3,  4,  5.5,  6.5,  7.5,  8.5,
                           9, 10, 11, 12, 13.5, 14.5, 15.5, 16.5};
        for (int i = 0; i < 16; i++)
        {
            M.m[i] = values[i];
        }

        assert(equal(M.m[0], 1) && equal(M.m[3], 4) && equal(M.m[4], 5.5) &&
               equal(M.m[6], 7.5) && equal(M.m[10], 11) &&
               equal(M.m[12], 13.5) && equal(M.m[14], 15.5));
    }

    { // Matrix equality with identical matrices
        matrix_t A, B;

        double valuesA[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        double valuesB[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2};
        for (int i = 0; i < 16; i++)
        {
            B.m[i] = valuesB[i];
        }

        assert(matrix_equal(A, B));
    }

    { // Matrix equality with different matrices
        matrix_t A, B;

        double valuesA[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        double valuesB[] = {2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        for (int i = 0; i < 16; i++)
        {
            B.m[i] = valuesB[i];
        }

        assert(!matrix_equal(A, B));
    }

    { // Multiplying two matrices
        matrix_t A, B;

        double valuesA[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        double valuesB[] = {-2, 1, 2, 3, 3, 2, 1, -1, 4, 3, 6, 5, 1, 2, 7, 8};
        for (int i = 0; i < 16; i++)
        {
            B.m[i] = valuesB[i];
        }

        matrix_t result = matrix_mul(A, B);

        matrix_t AB;

        double valuesAB[] = {20, 22, 50,  48,  44, 54, 114, 108,
                             40, 58, 110, 102, 16, 26, 46,  42};
        for (int i = 0; i < 16; i++)
        {
            AB.m[i] = valuesAB[i];
        }

        assert(matrix_equal(result, AB));
    }

    { // A matrix multiplied by a tuple
        matrix_t A;

        double valuesA[] = {1, 2, 3, 4, 2, 4, 4, 2, 8, 6, 4, 1, 0, 0, 0, 1};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        tuple_t b = {1, 2, 3, 1};

        tuple_t Ab = matrix_tmul(A, b);

        assert(equal(Ab.x, 18));
        assert(equal(Ab.y, 24));
        assert(equal(Ab.z, 33));
        assert(equal(Ab.w, 1));
    }

    { // Multiplying a matrix by the identity matrix
        matrix_t A;

        double valuesA[] = {0, 1, 2, 4, 1, 2, 3, 8, 2, 4, 8, 16, 4, 8, 16, 32};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        assert(matrix_equal(matrix_mul(A, IDENTITY), A));
    }

    { // Multiplying the identity matrix by a tuple
        tuple_t a = {1, 2, 3, 4};

        assert(tuple_equal(matrix_tmul(IDENTITY, a), a));
    }

    { // Transposing a matrix
        matrix_t A;

        double valuesA[] = {0, 9, 3, 0, 9, 8, 0, 8, 1, 8, 5, 3, 0, 0, 5, 8};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        matrix_t transpose;

        double valuesTranspose[] = {0, 9, 1, 0, 9, 8, 8, 0,
                                    3, 0, 5, 5, 0, 8, 3, 8};
        for (int i = 0; i < 16; i++)
        {
            transpose.m[i] = valuesTranspose[i];
        }

        assert(matrix_equal(matrix_transpose(A), transpose));
    }

    { // Transposing the identity matrix
        assert(matrix_equal(matrix_transpose(IDENTITY), IDENTITY));
    }

    { // Calculating the determinant of a 4x4 matrix
        matrix_t A;

        double valuesA[] = {-2, -8, 3,  5, -3, 1, 7, 3,
                            1,  2,  -9, 6, -6, 7, 7, -9};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        assert(equal(matrix_determinant(A), -4071));
    }

    { // Testing an invertible matrix for invertibility
        matrix_t A;

        double valuesA[] = {6, 4, 4, 4, 5, 5, 7, 6, 4, -9, 3, -7, 9, 1, 7, -6};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        assert(matrix_invertible(A));
    }

    { // Testing a noninvertible matrix for invertibility
        matrix_t A;

        double valuesA[] = {-4, 2,  -2, -3, 9, 6, 2, 6,
                            0,  -5, 1,  -5, 0, 0, 0, 0};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        assert(!matrix_invertible(A));
    }

    { // Calculating the inverse of a matrix
        matrix_t A;

        double valuesA[] = {-5, 2, 6,  -8, 1, -5, 1, 8,
                            7,  7, -6, -7, 1, -3, 7, 4};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        matrix_t B = matrix_inverse(A);

        matrix_t expected;

        double valuesExpected[] = {0.21805,  0.45113,  0.24060,  -0.04511,
                                   -0.80827, -1.45677, -0.44361, 0.52068,
                                   -0.07895, -0.22368, -0.05263, 0.19737,
                                   -0.52256, -0.81391, -0.30075, 0.30639};
        for (int i = 0; i < 16; i++)
        {
            expected.m[i] = valuesExpected[i];
        }

        assert(equal(matrix_determinant(A), 532.0));
        assert(matrix_equal(B, expected));
    }

    { // Calculating the inverse of another matrix
        matrix_t A;

        double valuesA[] = {8,  -5, 9, 2, 7,  5, 6,  1,
                            -6, 0,  9, 6, -3, 0, -9, -4};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        matrix_t B = matrix_inverse(A);

        matrix_t expected;

        double valuesExpected[] = {-0.15385, -0.15385, -0.28205, -0.53846,
                                   -0.07692, 0.12308,  0.02564,  0.03077,
                                   0.35897,  0.35897,  0.43590,  0.92308,
                                   -0.69231, -0.69231, -0.76923, -1.92308};
        for (int i = 0; i < 16; i++)
        {
            expected.m[i] = valuesExpected[i];
        }

        assert(matrix_equal(B, expected));
    }

    { // Calculating the inverse of a third matrix
        matrix_t A;

        double valuesA[] = {9,  3, 0, 9, -5, -2, -6, -3,
                            -4, 9, 6, 4, -7, 6,  6,  2};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        matrix_t B = matrix_inverse(A);

        matrix_t expected;

        double valuesExpected[] = {-0.04074, -0.07778, 0.14444,  -0.22222,
                                   -0.07778, 0.03333,  0.36667,  -0.33333,
                                   -0.02901, -0.14630, -0.10926, 0.12963,
                                   0.17778,  0.06667,  -0.26667, 0.33333};
        for (int i = 0; i < 16; i++)
        {
            expected.m[i] = valuesExpected[i];
        }

        assert(matrix_equal(B, expected));
    }

    {
        matrix_t A, B;

        double valuesA[] = {3,  -9, 7, 3, 3,  -8, 2,  -9,
                            -4, 4,  4, 1, -6, 5,  -1, 1};
        for (int i = 0; i < 16; i++)
        {
            A.m[i] = valuesA[i];
        }

        double valuesB[] = {8, 2, 2, 2, 3, -1, 7, 0, 7, 0, 5, 4, 6, -2, 0, 5};
        for (int i = 0; i < 16; i++)
        {
            B.m[i] = valuesB[i];
        }

        matrix_t C = matrix_mul(A, B);
        assert(matrix_equal(matrix_mul(C, matrix_inverse(B)), A));
    }
}

int main(void)
{
    test_matrices();

    return 0;
}
