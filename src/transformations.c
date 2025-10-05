// transformations.c

#include "../include/transformations.h"
#include <math.h>

matrix_t transform_translation(const double x, const double y, const double z)
{
    matrix_t t = IDENTITY;

    t.m[3]  = x;
    t.m[7]  = y;
    t.m[11] = z;

    return t;
}

matrix_t transform_scaling(const double x, const double y, const double z)
{
    matrix_t s = IDENTITY;

    s.m[0]  = x;
    s.m[5]  = y;
    s.m[10] = z;

    return s;
}

matrix_t transform_rotation_x(const double radians)
{
    matrix_t r = IDENTITY;

    r.m[5]  = cos(radians);
    r.m[6]  = -sin(radians);
    r.m[9]  = sin(radians);
    r.m[10] = cos(radians);

    return r;
}

matrix_t transform_rotation_y(const double radians)
{
    matrix_t r = IDENTITY;

    r.m[0]  = cos(radians);
    r.m[2]  = sin(radians);
    r.m[8]  = -sin(radians);
    r.m[10] = cos(radians);

    return r;
}

matrix_t transform_rotation_z(const double radians)
{
    matrix_t r = IDENTITY;

    r.m[0] = cos(radians);
    r.m[1] = -sin(radians);
    r.m[4] = sin(radians);
    r.m[5] = cos(radians);

    return r;
}

matrix_t transform_shearing(const double x_y, const double x_z,
                            const double y_x, const double y_z,
                            const double z_x, const double z_y)
{
    matrix_t s = IDENTITY;

    s.m[1] = x_y;
    s.m[2] = x_z;
    s.m[4] = y_x;
    s.m[6] = y_z;
    s.m[8] = z_x;
    s.m[9] = z_y;

    return s;
}

matrix_t transform_view(const tuple_t from, tuple_t to, tuple_t up)
{
    tuple_t direction = tuple_subtract(to, from);
    if (tuple_magnitude(direction) < EPSILON)
    {
        return IDENTITY;
    }

    tuple_t forward = tuple_normalize(direction);
    tuple_t upn     = tuple_normalize(up);
    tuple_t left    = tuple_cross(forward, upn);

    if (tuple_magnitude(left) < EPSILON)
    {
        if (fabs(forward.y) < 0.9)
            upn = vector(0, 1, 0);
        else
            upn = vector(1, 0, 0);

        left = tuple_cross(forward, upn);
    }

    tuple_t true_up = tuple_cross(left, forward);

    matrix_t orientation;

    orientation.m[0] = left.x;
    orientation.m[1] = left.y;
    orientation.m[2] = left.z;
    orientation.m[3] = 0;

    orientation.m[4] = true_up.x;
    orientation.m[5] = true_up.y;
    orientation.m[6] = true_up.z;
    orientation.m[7] = 0;

    orientation.m[8]  = -forward.x;
    orientation.m[9]  = -forward.y;
    orientation.m[10] = -forward.z;
    orientation.m[11] = 0;

    orientation.m[12] = 0;
    orientation.m[13] = 0;
    orientation.m[14] = 0;
    orientation.m[15] = 1;

    return matrix_mul(orientation,
                      transform_translation(-from.x, -from.y, -from.z));
}
