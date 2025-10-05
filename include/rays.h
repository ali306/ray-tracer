// rays.h

#ifndef RAYS_H
#define RAYS_H

#include "../include/transformations.h"
#include "../include/tuples.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    tuple_t origin;
    tuple_t direction;
} ray_t;

static inline ray_t ray(const tuple_t origin, const tuple_t direction)
{
    return (ray_t){origin, direction};
}

static inline tuple_t ray_position(const ray_t ray, const double t)
{
    return tuple_add(ray.origin, tuple_scale(ray.direction, t));
}

static inline ray_t ray_transform(ray_t r, matrix_t m)
{
    return ray(matrix_tmul(m, r.origin), matrix_tmul(m, r.direction));
}

#endif
