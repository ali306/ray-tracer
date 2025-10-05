// scenes.h

#ifndef SCENES_H
#define SCENES_H

#include "config.h"

#define WIDTH  DEFAULT_SCENE_WIDTH
#define HEIGHT WIDTH

#include "../include/camera.h"
#include "../include/canvas.h"
#include "../include/matrices.h"
#include "../include/tuples.h"
#include "../include/world.h"
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline uint64_t time(void)
{
    static mach_timebase_info_data_t info;
    if (info.denom == 0)
    {
        mach_timebase_info(&info);
    }
    uint64_t now = mach_absolute_time();
    return now * info.numer / info.denom;
}

bool scene_cover(void);
bool scene_checkered(void);
bool scene_reflect_refract(void);
bool scene_table(void);
bool scene_shadow_glamour(void);
bool scene_dragon(void);
bool scene_teapot(void);
bool scene_cornell_box(void);

#endif
