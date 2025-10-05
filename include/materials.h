// materials.h

#ifndef MATERIALS_H
#define MATERIALS_H

#include "lights.h"
#include "patterns.h"
#include "tuples.h"
#include <assert.h>

typedef struct
{
    tuple_t color;
    double ambient;
    double diffuse;
    double specular;
    double shininess;
    double reflective;
    double transparency;
    double refractive_index;
    pattern_t pattern;
    bool has_pattern;
    bool casts_shadow;
} material_t;

material_t material(void);

#endif
