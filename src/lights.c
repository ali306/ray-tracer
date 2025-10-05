// lights.c

#include "../include/lights.h"
#include "../include/sequences.h"
#include "../include/world.h"

light_t lights_point_light(const tuple_t position, const tuple_t intensity)
{
    sequence_t default_jitter = sequence_new();
    sequence_add(&default_jitter, 0.5);

    return (light_t){.intensity = intensity,
                     .position  = position,
                     .type      = LIGHT_POINT,
                     .corner    = point(0, 0, 0),
                     .uvec      = vector(0, 0, 0),
                     .vvec      = vector(0, 0, 0),
                     .usteps    = 1,
                     .vsteps    = 1,
                     .samples   = 1,
                     .jitter_by = default_jitter};
}

light_t lights_area_light(const tuple_t corner, const tuple_t full_uvec,
                          const int usteps, const tuple_t full_vvec,
                          const int vsteps, const tuple_t intensity)
{
    tuple_t uvec = tuple_scale(full_uvec, 1.0 / usteps);
    tuple_t vvec = tuple_scale(full_vvec, 1.0 / vsteps);
    int samples  = usteps * vsteps;

    tuple_t half_uvec = tuple_scale(full_uvec, 0.5);
    tuple_t half_vvec = tuple_scale(full_vvec, 0.5);
    tuple_t position  = tuple_add(tuple_add(corner, half_uvec), half_vvec);

    sequence_t default_jitter = sequence_new();
    sequence_add(&default_jitter, 0.5);

    return (light_t){.intensity = intensity,
                     .position  = position,
                     .type      = LIGHT_AREA,
                     .corner    = corner,
                     .uvec      = uvec,
                     .vvec      = vvec,
                     .usteps    = usteps,
                     .vsteps    = vsteps,
                     .samples   = samples,
                     .jitter_by = default_jitter};
}

tuple_t lights_point_on_light(const light_t *light, const int u, const int v)
{
    if (light == NULL)
    {
        return point(0, 0, 0);
    }

    int sample_index = v * light->usteps + u;
    double u_jitter  = sequence_at(&light->jitter_by, sample_index * 2);
    double v_jitter  = sequence_at(&light->jitter_by, sample_index * 2 + 1);

    tuple_t u_offset = tuple_scale(light->uvec, u + u_jitter);
    tuple_t v_offset = tuple_scale(light->vvec, v + v_jitter);
    return tuple_add(tuple_add(light->corner, u_offset), v_offset);
}

double lights_intensity_at(const light_t *light, const tuple_t point,
                           const world_t *world)
{
    if (light == NULL || world == NULL)
    {
        return 0.0;
    }

    if (light->type == LIGHT_POINT)
    {
        if (world_is_shadowed(world, light->position, point))
        {
            return 0.0;
        }
        return 1.0;
    }
    else if (light->type == LIGHT_AREA)
    {
        double total = 0.0;

        for (int v = 0; v < light->vsteps; v++)
        {
            for (int u = 0; u < light->usteps; u++)
            {
                tuple_t light_position = lights_point_on_light(light, u, v);
                if (!world_is_shadowed(world, light_position, point))
                {
                    total += 1.0;
                }
            }
        }

        return total / light->samples;
    }

    return 0.0;
}
