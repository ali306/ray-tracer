// shapes.h

#ifndef SHAPES_H
#define SHAPES_H

#include "../include/intersections.h"
#include "../include/lights.h"
#include "../include/materials.h"
#include "../include/matrices.h"
#include "../include/patterns.h"
#include "../include/rays.h"
#include "../include/transformations.h"
#include "config.h"
#include <assert.h>
#include <stdbool.h>

typedef struct
{
    tuple_t min;
    tuple_t max;
} bounding_box_t;

typedef enum
{
    SHAPE_TEST,
    SHAPE_SPHERE,
    SHAPE_PLANE,
    SHAPE_CUBE,
    SHAPE_CYLINDER,
    SHAPE_CONE,
    SHAPE_TRIANGLE,
    SHAPE_SMOOTH_TRIANGLE,
    SHAPE_GROUP
} shape_type_t;

typedef struct
{
    shape_type_t type;
    matrix_t transform;
    matrix_t inverse_transform;
    matrix_t transposed_inverse_transform;
    material_t material;
    void *parent;
    bounding_box_t world_bounds;
    bool world_bounds_cached;
} shape_t;

typedef shape_t sphere_t;
typedef shape_t plane_t;
typedef shape_t cube_t;

typedef struct
{
    shape_type_t type;
    matrix_t transform;
    matrix_t inverse_transform;
    matrix_t transposed_inverse_transform;
    material_t material;
    void *parent;
    bounding_box_t world_bounds;
    bool world_bounds_cached;
    double minimum;
    double maximum;
    bool closed;
} cylinder_t;

typedef struct
{
    shape_type_t type;
    matrix_t transform;
    matrix_t inverse_transform;
    matrix_t transposed_inverse_transform;
    material_t material;
    void *parent;
    bounding_box_t world_bounds;
    bool world_bounds_cached;
    double minimum;
    double maximum;
    bool closed;
} cone_t;

typedef struct
{
    shape_type_t type;
    matrix_t transform;
    matrix_t inverse_transform;
    matrix_t transposed_inverse_transform;
    material_t material;
    void *parent;
    bounding_box_t world_bounds;
    bool world_bounds_cached;
    ray_t saved_ray;
    bool has_saved_ray;
} test_shape_t;

typedef struct
{
    shape_type_t type;
    matrix_t transform;
    matrix_t inverse_transform;
    matrix_t transposed_inverse_transform;
    material_t material;
    void *parent;
    bounding_box_t world_bounds;
    bool world_bounds_cached;
    tuple_t p1, p2, p3;
    tuple_t e1, e2;
    tuple_t normal;
} triangle_t;

typedef struct
{
    shape_type_t type;
    matrix_t transform;
    matrix_t inverse_transform;
    matrix_t transposed_inverse_transform;
    material_t material;
    void *parent;
    bounding_box_t world_bounds;
    bool world_bounds_cached;
    tuple_t p1, p2, p3;
    tuple_t e1, e2;
    tuple_t normal;
    tuple_t n1, n2, n3;
} smooth_triangle_t;

typedef struct group_s group_t;

struct group_s
{
    shape_type_t type;
    matrix_t transform;
    matrix_t inverse_transform;
    matrix_t transposed_inverse_transform;
    material_t material;
    void *parent;
    bounding_box_t world_bounds;
    bool world_bounds_cached;
    shape_t **children;
    unsigned child_count;
    unsigned children_capacity;
    tuple_t cached_bounds_min;
    tuple_t cached_bounds_max;
    bool bounds_cached;
};

void shape(shape_t *shape, const shape_type_t type);
void shape_set_transform(shape_t *shape, const matrix_t m);
tuple_t shape_normal_at(const shape_t *shape, const tuple_t world_point,
                        const intersection_t *hit);
tuple_t normal_at(const void *shape, const tuple_t world_point,
                  const intersection_t *hit);
intersections_t shape_intersect(const shape_t *shape, const ray_t r);

sphere_t sphere(void);
sphere_t glass_sphere(void);
void sphere_set_transform(sphere_t *s, matrix_t m);
intersections_t sphere_intersect(const sphere_t *s, const ray_t r);

plane_t plane(void);

static inline intersections_t plane_intersect(const plane_t *p, ray_t r)
{
    if (p == NULL)
    {
        return empty_intersections();
    }

    if (fabs(r.direction.y) < EPSILON)
    {
        return empty_intersections();
    }

    double t = -r.origin.y / r.direction.y;

    return (intersections_t){.count         = 1,
                             .intersections = {intersection(t, (void *)p)}};
}

tuple_t pattern_at_shape(pattern_t pattern, shape_t object,
                         tuple_t world_point);
static inline tuple_t materials_lighting(const material_t *m, const shape_t *o,
                                         const light_t *l, const tuple_t p,
                                         const tuple_t eyev,
                                         const tuple_t normalv,
                                         const double intensity)
{
    if (m == NULL || o == NULL || l == NULL)
    {
        return color(0, 0, 0);
    }

    tuple_t c;

    if (m->has_pattern)
    {
        c = pattern_at_shape(m->pattern, *o, p);
    }
    else
    {
        c = m->color;
    }

    tuple_t effective_color = tuple_hadamard(c, l->intensity);
    tuple_t ambient         = tuple_scale(effective_color, m->ambient);

    tuple_t sum = color(0, 0, 0);

    if (l->type == LIGHT_POINT)
    {
        tuple_t light_vector    = tuple_subtract(l->position, p);
        tuple_t lightv          = tuple_normalize(light_vector);
        double light_dot_normal = tuple_dot(lightv, normalv);

        if (light_dot_normal > 0)
        {
            tuple_t diffuse =
                tuple_scale(effective_color, m->diffuse * light_dot_normal);
            sum = tuple_add(sum, diffuse);

            tuple_t reflectv = tuple_reflect(tuple_negate(lightv), normalv);
            double reflect_dot_eye = tuple_dot(reflectv, eyev);

            if (reflect_dot_eye > 0)
            {
                double factor = pow(reflect_dot_eye, m->shininess);
                tuple_t specular =
                    tuple_scale(l->intensity, m->specular * factor);
                sum = tuple_add(sum, specular);
            }
        }
    }
    else if (l->type == LIGHT_AREA)
    {
        for (int v = 0; v < l->vsteps; v++)
        {
            for (int u = 0; u < l->usteps; u++)
            {
                tuple_t light_position =
                    lights_point_on_light((light_t *)l, u, v);
                tuple_t light_vector    = tuple_subtract(light_position, p);
                tuple_t lightv          = tuple_normalize(light_vector);
                double light_dot_normal = tuple_dot(lightv, normalv);

                if (light_dot_normal > 0)
                {
                    tuple_t diffuse = tuple_scale(
                        effective_color, m->diffuse * light_dot_normal);
                    sum = tuple_add(sum, diffuse);

                    tuple_t reflectv =
                        tuple_reflect(tuple_negate(lightv), normalv);
                    double reflect_dot_eye = tuple_dot(reflectv, eyev);

                    if (reflect_dot_eye > 0)
                    {
                        double factor = pow(reflect_dot_eye, m->shininess);
                        tuple_t specular =
                            tuple_scale(l->intensity, m->specular * factor);
                        sum = tuple_add(sum, specular);
                    }
                }
            }
        }
        sum = tuple_scale(sum, 1.0 / l->samples);
    }

    sum = tuple_scale(sum, intensity);

    return tuple_add(ambient, sum);
}

cube_t cube(void);

cylinder_t cylinder(void);
intersections_t cylinder_intersect(const cylinder_t *c, ray_t r);

cone_t cone(void);
intersections_t cone_intersect(const cone_t *c, ray_t r);

triangle_t triangle(tuple_t p1, tuple_t p2, tuple_t p3);
intersections_t triangle_intersect(const triangle_t *t, ray_t r);

smooth_triangle_t smooth_triangle(tuple_t p1, tuple_t p2, tuple_t p3,
                                  tuple_t n1, tuple_t n2, tuple_t n3);
intersections_t smooth_triangle_intersect(const smooth_triangle_t *t, ray_t r);

group_t *group(void);
void group_free(group_t *g);
void group_add_child(group_t *g, shape_t *s);
bool group_includes(const group_t *g, const shape_t *s);
intersections_t group_intersect(const group_t *g, ray_t r);
intersections_t group_local_intersect(const group_t *g, ray_t r);

tuple_t world_to_object(const shape_t *shape, tuple_t point);
tuple_t normal_to_world(const shape_t *shape, tuple_t normal);

typedef struct
{
    shape_t **shapes;
    unsigned count;
    unsigned capacity;
} shape_list_t;

shape_list_t shape_list_create(void);
void shape_list_free(shape_list_t *list);
void shape_list_add(shape_list_t *list, shape_t *shape);
void partition_children(group_t *group, shape_list_t *left,
                        shape_list_t *right);
void make_subgroup(group_t *parent_group, shape_list_t *children);
void divide(shape_t *shape, unsigned threshold);
void divide_recursive(shape_t *shape, unsigned threshold, unsigned max_depth);

#define group_is_empty(g) ((g)->child_count == 0)

static inline void check_axis(const double origin, const double direction,
                              double *tmin, double *tmax)
{
    double tmin_numerator = (-1 - origin);
    double tmax_numerator = (1 - origin);

    if (fabs(direction) >= EPSILON)
    {
        *tmin = tmin_numerator / direction;
        *tmax = tmax_numerator / direction;
    }
    else
    {
        *tmin = tmin_numerator > 0 ? 1e6 : -1e6;
        *tmax = tmax_numerator > 0 ? 1e6 : -1e6;
    }

    if (*tmin > *tmax)
    {
        double temp = *tmin;
        *tmin       = *tmax;
        *tmax       = temp;
    }
}

intersections_t cube_intersect(const cube_t *c, ray_t r);

test_shape_t test_shape(void);
intersections_t test_shape_intersect(test_shape_t *t, ray_t r);

#endif
