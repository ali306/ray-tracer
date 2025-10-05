// matrices.c

#include "../include/matrices.h"

const matrix_t IDENTITY = {
    .m = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}};

matrix_t matrix_transpose(const matrix_t a)
{
    matrix_t output;

    output.m[0] = a.m[0];
    output.m[1] = a.m[4];
    output.m[2] = a.m[8];
    output.m[3] = a.m[12];

    output.m[4] = a.m[1];
    output.m[5] = a.m[5];
    output.m[6] = a.m[9];
    output.m[7] = a.m[13];

    output.m[8]  = a.m[2];
    output.m[9]  = a.m[6];
    output.m[10] = a.m[10];
    output.m[11] = a.m[14];

    output.m[12] = a.m[3];
    output.m[13] = a.m[7];
    output.m[14] = a.m[11];
    output.m[15] = a.m[15];

    return output;
}

double matrix_determinant(const matrix_t a)
{
    double m0 = a.m[5] * (a.m[10] * a.m[15] - a.m[11] * a.m[14]) -
                a.m[6] * (a.m[9] * a.m[15] - a.m[11] * a.m[13]) +
                a.m[7] * (a.m[9] * a.m[14] - a.m[10] * a.m[13]);

    double m1 = a.m[4] * (a.m[10] * a.m[15] - a.m[11] * a.m[14]) -
                a.m[6] * (a.m[8] * a.m[15] - a.m[11] * a.m[12]) +
                a.m[7] * (a.m[8] * a.m[14] - a.m[10] * a.m[12]);

    double m2 = a.m[4] * (a.m[9] * a.m[15] - a.m[11] * a.m[13]) -
                a.m[5] * (a.m[8] * a.m[15] - a.m[11] * a.m[12]) +
                a.m[7] * (a.m[8] * a.m[13] - a.m[9] * a.m[12]);

    double m3 = a.m[4] * (a.m[9] * a.m[14] - a.m[10] * a.m[13]) -
                a.m[5] * (a.m[8] * a.m[14] - a.m[10] * a.m[12]) +
                a.m[6] * (a.m[8] * a.m[13] - a.m[9] * a.m[12]);

    return a.m[0] * m0 - a.m[1] * m1 + a.m[2] * m2 - a.m[3] * m3;
}

bool matrix_invertible(const matrix_t a)
{
    return !equal(matrix_determinant(a), 0);
}

matrix_t matrix_inverse(const matrix_t a)
{
    matrix_t output;

    // Calculate temporary values for the first two rows of cofactors
    const double a00 = a.m[0], a01 = a.m[1], a02 = a.m[2], a03 = a.m[3];
    const double a10 = a.m[4], a11 = a.m[5], a12 = a.m[6], a13 = a.m[7];
    const double a20 = a.m[8], a21 = a.m[9], a22 = a.m[10], a23 = a.m[11];
    const double a30 = a.m[12], a31 = a.m[13], a32 = a.m[14], a33 = a.m[15];

    // Calculate cofactors
    const double b00 = a11 * (a22 * a33 - a23 * a32) -
                       a12 * (a21 * a33 - a23 * a31) +
                       a13 * (a21 * a32 - a22 * a31);
    const double b01 = a10 * (a22 * a33 - a23 * a32) -
                       a12 * (a20 * a33 - a23 * a30) +
                       a13 * (a20 * a32 - a22 * a30);
    const double b02 = a10 * (a21 * a33 - a23 * a31) -
                       a11 * (a20 * a33 - a23 * a30) +
                       a13 * (a20 * a31 - a21 * a30);
    const double b03 = a10 * (a21 * a32 - a22 * a31) -
                       a11 * (a20 * a32 - a22 * a30) +
                       a12 * (a20 * a31 - a21 * a30);

    const double det = a00 * b00 - a01 * b01 + a02 * b02 - a03 * b03;

    if (equal(det, 0.0))
    {
        return IDENTITY;
    }

    const double inv_det = 1.0 / det;

    const double b10 = a01 * (a22 * a33 - a23 * a32) -
                       a02 * (a21 * a33 - a23 * a31) +
                       a03 * (a21 * a32 - a22 * a31);
    const double b11 = a00 * (a22 * a33 - a23 * a32) -
                       a02 * (a20 * a33 - a23 * a30) +
                       a03 * (a20 * a32 - a22 * a30);
    const double b12 = a00 * (a21 * a33 - a23 * a31) -
                       a01 * (a20 * a33 - a23 * a30) +
                       a03 * (a20 * a31 - a21 * a30);
    const double b13 = a00 * (a21 * a32 - a22 * a31) -
                       a01 * (a20 * a32 - a22 * a30) +
                       a02 * (a20 * a31 - a21 * a30);

    const double b20 = a01 * (a12 * a33 - a13 * a32) -
                       a02 * (a11 * a33 - a13 * a31) +
                       a03 * (a11 * a32 - a12 * a31);
    const double b21 = a00 * (a12 * a33 - a13 * a32) -
                       a02 * (a10 * a33 - a13 * a30) +
                       a03 * (a10 * a32 - a12 * a30);
    const double b22 = a00 * (a11 * a33 - a13 * a31) -
                       a01 * (a10 * a33 - a13 * a30) +
                       a03 * (a10 * a31 - a11 * a30);
    const double b23 = a00 * (a11 * a32 - a12 * a31) -
                       a01 * (a10 * a32 - a12 * a30) +
                       a02 * (a10 * a31 - a11 * a30);

    const double b30 = a01 * (a12 * a23 - a13 * a22) -
                       a02 * (a11 * a23 - a13 * a21) +
                       a03 * (a11 * a22 - a12 * a21);
    const double b31 = a00 * (a12 * a23 - a13 * a22) -
                       a02 * (a10 * a23 - a13 * a20) +
                       a03 * (a10 * a22 - a12 * a20);
    const double b32 = a00 * (a11 * a23 - a13 * a21) -
                       a01 * (a10 * a23 - a13 * a20) +
                       a03 * (a10 * a21 - a11 * a20);
    const double b33 = a00 * (a11 * a22 - a12 * a21) -
                       a01 * (a10 * a22 - a12 * a20) +
                       a02 * (a10 * a21 - a11 * a20);

    output.m[0]  = b00 * inv_det;
    output.m[1]  = -b10 * inv_det;
    output.m[2]  = b20 * inv_det;
    output.m[3]  = -b30 * inv_det;
    output.m[4]  = -b01 * inv_det;
    output.m[5]  = b11 * inv_det;
    output.m[6]  = -b21 * inv_det;
    output.m[7]  = b31 * inv_det;
    output.m[8]  = b02 * inv_det;
    output.m[9]  = -b12 * inv_det;
    output.m[10] = b22 * inv_det;
    output.m[11] = -b32 * inv_det;
    output.m[12] = -b03 * inv_det;
    output.m[13] = b13 * inv_det;
    output.m[14] = -b23 * inv_det;
    output.m[15] = b33 * inv_det;

    return output;
}

bool matrix_equal(matrix_t a, matrix_t b)
{
    if (!equal(a.m[0], b.m[0]))
        return false;
    if (!equal(a.m[1], b.m[1]))
        return false;
    if (!equal(a.m[2], b.m[2]))
        return false;
    if (!equal(a.m[3], b.m[3]))
        return false;
    if (!equal(a.m[4], b.m[4]))
        return false;
    if (!equal(a.m[5], b.m[5]))
        return false;
    if (!equal(a.m[6], b.m[6]))
        return false;
    if (!equal(a.m[7], b.m[7]))
        return false;
    if (!equal(a.m[8], b.m[8]))
        return false;
    if (!equal(a.m[9], b.m[9]))
        return false;
    if (!equal(a.m[10], b.m[10]))
        return false;
    if (!equal(a.m[11], b.m[11]))
        return false;
    if (!equal(a.m[12], b.m[12]))
        return false;
    if (!equal(a.m[13], b.m[13]))
        return false;
    if (!equal(a.m[14], b.m[14]))
        return false;
    if (!equal(a.m[15], b.m[15]))
        return false;

    return true;
}
