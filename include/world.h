// world.h

#ifndef WORLD_H
#define WORLD_H

#include "config.h"
#include "intersections.h"
#include "lights.h"
#include "shapes.h"

typedef union
{
    shape_t shape;
    sphere_t sphere;
    plane_t plane;
    cube_t cube;
    cylinder_t cylinder;
    triangle_t triangle;
    smooth_triangle_t smooth_triangle;
    group_t group;
} object_t;

typedef struct
{
    object_t *objects;
    unsigned object_count;
    unsigned object_capacity;
    light_t *lights;
    unsigned light_count;
    unsigned light_capacity;
} world_t;

world_t world(void);

world_t world_default(void);

void world_free(world_t *w);

void world_intersect(const world_t *w, const ray_t *r, intersections_t *xs);

tuple_t world_shade_hit(const world_t *w, const computations_t *c,
                        const unsigned remaining);

tuple_t world_color_at(const world_t *w, const ray_t *r,
                       const unsigned remaining);

bool world_is_shadowed(const world_t *w, const tuple_t light_position,
                       const tuple_t p);

void world_add_shape(world_t *w, shape_t s);
void world_add_cylinder(world_t *w, cylinder_t c);
void world_add_triangle(world_t *w, triangle_t t);
void world_add_smooth_triangle(world_t *w, smooth_triangle_t t);
void world_add_group(world_t *w, group_t *g);
void world_add_light(world_t *w, light_t light);

tuple_t world_reflected_color(const world_t *w, const computations_t *c,
                              const unsigned remaining);

tuple_t world_refracted_color(const world_t *w, const computations_t *c,
                              const unsigned remaining);

double lights_intensity_at(const light_t *light, const tuple_t point,
                           const world_t *world);

#endif
