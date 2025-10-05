// patterns.c

#include "../include/patterns.h"
#include <math.h>
#include <stdio.h>

pattern_t pattern(const pattern_type_t type)
{
    pattern_t pattern = {.type              = type,
                         .transform         = IDENTITY,
                         .inverse_transform = IDENTITY,
                         .a                 = WHITE,
                         .b                 = BLACK};
    return pattern;
}

void patterns_set_transform(pattern_t *p, const matrix_t t)
{
    if (p == NULL)
    {
        return;
    }

    p->transform         = t;
    p->inverse_transform = matrix_inverse(t);
}

tuple_t pattern_at(const pattern_t p, const tuple_t point)
{
    switch (p.type)
    {
    case PATTERN_TEST:
        return color(point.x, point.y, point.z);

    case PATTERN_STRIPE:
        if ((int)floor(point.x) % 2 == 0)
        {
            return p.a;
        }
        else
        {
            return p.b;
        }

    case PATTERN_GRADIENT:
    {
        tuple_t distance = tuple_subtract(p.b, p.a);
        double fraction  = point.x - floor(point.x);
        return tuple_add(p.a, tuple_scale(distance, fraction));
    }

    case PATTERN_RING:
        if ((int)floor(sqrt(point.x * point.x + point.z * point.z)) % 2 == 0)
        {
            return p.a;
        }
        else
        {
            return p.b;
        }

    case PATTERN_CHECKER:
    {
        int sum = (int)(floor(point.x) + floor(point.y) + floor(point.z));
        if (sum % 2 == 0)
        {
            return p.a;
        }
        else
        {
            return p.b;
        }
    }

    default:
        return BLACK;
    }
}

pattern_t patterns_test(void)
{
    return pattern(PATTERN_TEST);
}

pattern_t patterns_stripe(const tuple_t a, const tuple_t b)
{
    pattern_t p = pattern(PATTERN_STRIPE);
    p.a         = a;
    p.b         = b;
    return p;
}

pattern_t patterns_gradient(const tuple_t a, const tuple_t b)
{
    pattern_t p = pattern(PATTERN_GRADIENT);
    p.a         = a;
    p.b         = b;
    return p;
}

pattern_t patterns_ring(const tuple_t a, const tuple_t b)
{
    pattern_t p = pattern(PATTERN_RING);
    p.a         = a;
    p.b         = b;
    return p;
}

pattern_t patterns_checker(const tuple_t a, const tuple_t b)
{
    pattern_t p = pattern(PATTERN_CHECKER);
    p.a         = a;
    p.b         = b;
    return p;
}
