// material.c

#include "../include/materials.h"

material_t material(void)
{
    return (material_t){.color            = color(1, 1, 1),
                        .ambient          = 0.1,
                        .diffuse          = 0.9,
                        .specular         = 0.9,
                        .shininess        = 200.0,
                        .reflective       = 0.0,
                        .transparency     = 0.0,
                        .refractive_index = 1.0,
                        .has_pattern      = false,
                        .casts_shadow     = true};
}
