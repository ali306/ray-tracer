// matrices.h

#include "../include/tuples.h"
#include <stdbool.h>

#ifndef MATRICES_H
#define MATRICES_H

typedef struct
{
    double m[16];
} matrix_t;

extern const matrix_t IDENTITY;

static inline matrix_t matrix_mul(const matrix_t a, const matrix_t b)
{
    matrix_t output;

    // Row 0
    output.m[0] =
        a.m[0] * b.m[0] + a.m[1] * b.m[4] + a.m[2] * b.m[8] + a.m[3] * b.m[12];
    output.m[1] =
        a.m[0] * b.m[1] + a.m[1] * b.m[5] + a.m[2] * b.m[9] + a.m[3] * b.m[13];
    output.m[2] =
        a.m[0] * b.m[2] + a.m[1] * b.m[6] + a.m[2] * b.m[10] + a.m[3] * b.m[14];
    output.m[3] =
        a.m[0] * b.m[3] + a.m[1] * b.m[7] + a.m[2] * b.m[11] + a.m[3] * b.m[15];

    // Row 1
    output.m[4] =
        a.m[4] * b.m[0] + a.m[5] * b.m[4] + a.m[6] * b.m[8] + a.m[7] * b.m[12];
    output.m[5] =
        a.m[4] * b.m[1] + a.m[5] * b.m[5] + a.m[6] * b.m[9] + a.m[7] * b.m[13];
    output.m[6] =
        a.m[4] * b.m[2] + a.m[5] * b.m[6] + a.m[6] * b.m[10] + a.m[7] * b.m[14];
    output.m[7] =
        a.m[4] * b.m[3] + a.m[5] * b.m[7] + a.m[6] * b.m[11] + a.m[7] * b.m[15];

    // Row 2
    output.m[8] = a.m[8] * b.m[0] + a.m[9] * b.m[4] + a.m[10] * b.m[8] +
                  a.m[11] * b.m[12];
    output.m[9] = a.m[8] * b.m[1] + a.m[9] * b.m[5] + a.m[10] * b.m[9] +
                  a.m[11] * b.m[13];
    output.m[10] = a.m[8] * b.m[2] + a.m[9] * b.m[6] + a.m[10] * b.m[10] +
                   a.m[11] * b.m[14];
    output.m[11] = a.m[8] * b.m[3] + a.m[9] * b.m[7] + a.m[10] * b.m[11] +
                   a.m[11] * b.m[15];

    // Row 3
    output.m[12] = a.m[12] * b.m[0] + a.m[13] * b.m[4] + a.m[14] * b.m[8] +
                   a.m[15] * b.m[12];
    output.m[13] = a.m[12] * b.m[1] + a.m[13] * b.m[5] + a.m[14] * b.m[9] +
                   a.m[15] * b.m[13];
    output.m[14] = a.m[12] * b.m[2] + a.m[13] * b.m[6] + a.m[14] * b.m[10] +
                   a.m[15] * b.m[14];
    output.m[15] = a.m[12] * b.m[3] + a.m[13] * b.m[7] + a.m[14] * b.m[11] +
                   a.m[15] * b.m[15];

    return output;
}

static inline tuple_t matrix_tmul(const matrix_t a, const tuple_t b)
{
    tuple_t output;

    output.x = a.m[0] * b.x + a.m[1] * b.y + a.m[2] * b.z + a.m[3] * b.w;
    output.y = a.m[4] * b.x + a.m[5] * b.y + a.m[6] * b.z + a.m[7] * b.w;
    output.z = a.m[8] * b.x + a.m[9] * b.y + a.m[10] * b.z + a.m[11] * b.w;
    output.w = a.m[12] * b.x + a.m[13] * b.y + a.m[14] * b.z + a.m[15] * b.w;

    return output;
}

matrix_t matrix_transpose(const matrix_t a);

double matrix_determinant(const matrix_t a);

bool matrix_invertible(const matrix_t a);

matrix_t matrix_inverse(const matrix_t a);

bool matrix_equal(matrix_t a, matrix_t b);

#endif
