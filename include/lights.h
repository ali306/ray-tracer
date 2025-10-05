// lights.h

#ifndef LIGHTS_H
#define LIGHTS_H

#include "sequences.h"
#include "tuples.h"
#include <assert.h>

typedef enum
{
    LIGHT_POINT,
    LIGHT_AREA
} light_type_t;

typedef struct
{
    tuple_t intensity;
    tuple_t position;
    light_type_t type;

    tuple_t corner;
    tuple_t uvec;
    tuple_t vvec;
    int usteps;
    int vsteps;
    int samples;
    sequence_t jitter_by;
} light_t;

light_t lights_point_light(const tuple_t position, const tuple_t intensity);

light_t lights_area_light(const tuple_t corner, const tuple_t full_uvec,
                          const int usteps, const tuple_t full_vvec,
                          const int vsteps, const tuple_t intensity);

tuple_t lights_point_on_light(const light_t *light, const int u, const int v);

#endif
