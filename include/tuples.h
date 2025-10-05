// tuples.h

#ifndef TUPLES_H
#define TUPLES_H

#include "config.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct
{
    double x, y, z, w;
} tuple_t;

static inline tuple_t vector(const double x, const double y, const double z)
{
    return (tuple_t){x, y, z, 0};
}

static inline tuple_t point(const double x, const double y, const double z)
{
    return (tuple_t){x, y, z, 1};
}

static inline bool equal(const double a, const double b)
{
    return fabs(a - b) < EPSILON;
}

static inline bool tuple_equal(const tuple_t a, const tuple_t b)
{
    return equal(a.x, b.x) && equal(a.y, b.y) && equal(a.z, b.z) &&
           equal(a.w, b.w);
}

static inline tuple_t tuple_add(const tuple_t a, const tuple_t b)
{
    return (tuple_t){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

static inline tuple_t tuple_subtract(const tuple_t a, const tuple_t b)
{
    return (tuple_t){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

static inline tuple_t tuple_negate(const tuple_t a)
{
    return (tuple_t){-a.x, -a.y, -a.z, -a.w};
}

static inline tuple_t tuple_scale(const tuple_t a, const double n)
{
    return (tuple_t){a.x * n, a.y * n, a.z * n, a.w * n};
}

static inline tuple_t tuple_scalar_divide(const tuple_t a, const double n)
{
    if (fabs(n) < EPSILON)
    {
        return (tuple_t){0.0, 0.0, 0.0, 0.0};
    }
    return (tuple_t){a.x / n, a.y / n, a.z / n, a.w / n};
}

static inline double tuple_magnitude(const tuple_t a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

static inline tuple_t tuple_normalize(const tuple_t a)
{
    double mag = tuple_magnitude(a);
    if (mag > EPSILON)
    {
        return tuple_scale(a, 1 / mag);
    }
    return (tuple_t){0.0, 0.0, 0.0, 0.0};
}

static inline double tuple_dot(const tuple_t a, const tuple_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static inline tuple_t tuple_cross(const tuple_t a, const tuple_t b)
{
    return (tuple_t){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                     a.x * b.y - a.y * b.x, 0.0};
}

static inline tuple_t color(const double red, const double green,
                            const double blue)
{
    return (tuple_t){.x = red, .y = green, .z = blue, .w = 0.0};
}

static inline tuple_t tuple_hadamard(const tuple_t a, const tuple_t b)
{
    return (tuple_t){a.x * b.x, a.y * b.y, a.z * b.z, 0.0};
}

static inline tuple_t tuple_reflect(const tuple_t in, const tuple_t normal)
{
    return tuple_subtract(in, tuple_scale(normal, 2 * tuple_dot(in, normal)));
}

static inline tuple_t hex_color(const char *hex_str)
{
    if (hex_str == NULL)
    {
        fprintf(stderr, "Warning: hex_color received NULL input.\n");
        return color(0, 0, 0);
    }

    const char *str = hex_str;
    if (*str == '#')
    {
        str++;
    }

    unsigned int r = 0, g = 0, b = 0;
    if (sscanf(str, "%02x%02x%02x", &r, &g, &b) == 3)
    {
        double red   = r / 255.0;
        double green = g / 255.0;
        double blue  = b / 255.0;
        return color(red, green, blue);
    }
    else
    {
        fprintf(stderr, "Warning: hex_color failed to parse '%s'.\n", hex_str);
        return color(0, 0, 0);
    }
}

#endif
