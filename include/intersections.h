// intersections.h

#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include "../include/rays.h"
#include "../include/tuples.h"
#include "config.h"
#include <assert.h>

typedef struct
{
    double t;
    void *object;
    double u;
    double v;
} intersection_t;

typedef struct
{
    int count;
    intersection_t intersections[MAX_INTERSECTIONS];
} intersections_t;

typedef struct
{
    double t;
    void *object;
    tuple_t point;
    tuple_t eyev;
    tuple_t normalv;
    tuple_t reflectv;
    bool inside;
    tuple_t over_point;
    tuple_t under_point;
    double n1;
    double n2;
} computations_t;

intersection_t intersection(double t, void *object);

intersection_t intersection_with_uv(double t, void *object, double u, double v);

intersections_t intersections(int count, ...);

intersection_t *intersections_hit(const intersections_t *xs);

computations_t intersections_prepare_computations(const intersection_t *i,
                                                  const ray_t *r,
                                                  const intersections_t *xs);

double intersections_shlick(const computations_t *c);

void intersections_sort(intersections_t *xs);

static inline intersections_t empty_intersections(void)
{
    intersections_t result;
    result.count = 0;
    return result;
}

#endif
