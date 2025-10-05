#include "../../include/bounds.h"
#include "../../include/shapes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

triangle_t triangle(tuple_t p1, tuple_t p2, tuple_t p3)
{
    triangle_t t;
    t.type                         = SHAPE_TRIANGLE;
    t.transform                    = IDENTITY;
    t.inverse_transform            = IDENTITY;
    t.transposed_inverse_transform = IDENTITY;
    t.material                     = material();
    t.parent                       = NULL;

    t.p1 = p1;
    t.p2 = p2;
    t.p3 = p3;

    t.e1 = tuple_subtract(p2, p1);
    t.e2 = tuple_subtract(p3, p1);

    t.normal = tuple_normalize(tuple_cross(t.e2, t.e1));

    bounding_box_t local_bounds = bounds_of_triangle(t);
    t.world_bounds              = local_bounds;
    t.world_bounds_cached       = true;

    return t;
}

smooth_triangle_t smooth_triangle(tuple_t p1, tuple_t p2, tuple_t p3,
                                  tuple_t n1, tuple_t n2, tuple_t n3)
{
    smooth_triangle_t t;
    t.type                         = SHAPE_SMOOTH_TRIANGLE;
    t.transform                    = IDENTITY;
    t.inverse_transform            = IDENTITY;
    t.transposed_inverse_transform = IDENTITY;
    t.material                     = material();
    t.parent                       = NULL;

    t.p1 = p1;
    t.p2 = p2;
    t.p3 = p3;

    t.e1 = tuple_subtract(p2, p1);
    t.e2 = tuple_subtract(p3, p1);

    t.normal = tuple_normalize(tuple_cross(t.e2, t.e1));

    t.n1 = n1;
    t.n2 = n2;
    t.n3 = n3;

    bounding_box_t local_bounds = bounds_of_triangle(*(triangle_t *)&t);
    t.world_bounds              = local_bounds;
    t.world_bounds_cached       = true;

    return t;
}

intersections_t triangle_intersect(const triangle_t *t, ray_t r)
{
    if (t == NULL)
    {
        return empty_intersections();
    }

    tuple_t dir_cross_e2 = tuple_cross(r.direction, t->e2);
    double det           = tuple_dot(t->e1, dir_cross_e2);

    if (fabs(det) < EPSILON)
    {
        return empty_intersections();
    }

    double f             = 1.0 / det;
    tuple_t p1_to_origin = tuple_subtract(r.origin, t->p1);
    double u             = f * tuple_dot(p1_to_origin, dir_cross_e2);

    if (u < 0 || u > 1)
    {
        return empty_intersections();
    }

    tuple_t origin_cross_e1 = tuple_cross(p1_to_origin, t->e1);
    double v                = f * tuple_dot(r.direction, origin_cross_e1);

    if (v < 0 || (u + v) > 1)
    {
        return empty_intersections();
    }

    double ray_t = f * tuple_dot(t->e2, origin_cross_e1);

    intersections_t result;
    result.count            = 1;
    result.intersections[0] = intersection_with_uv(ray_t, (void *)t, u, v);
    return result;
}

intersections_t smooth_triangle_intersect(const smooth_triangle_t *t, ray_t r)
{
    if (t == NULL)
    {
        return empty_intersections();
    }
    tuple_t dir_cross_e2 = tuple_cross(r.direction, t->e2);
    double det           = tuple_dot(t->e1, dir_cross_e2);
    if (fabs(det) < EPSILON)
    {
        return empty_intersections();
    }
    double f             = 1.0 / det;
    tuple_t p1_to_origin = tuple_subtract(r.origin, t->p1);
    double u             = f * tuple_dot(p1_to_origin, dir_cross_e2);
    if (u < 0 || u > 1)
    {
        return empty_intersections();
    }
    tuple_t origin_cross_e1 = tuple_cross(p1_to_origin, t->e1);
    double v                = f * tuple_dot(r.direction, origin_cross_e1);
    if (v < 0 || (u + v) > 1)
    {
        return empty_intersections();
    }
    double ray_t = f * tuple_dot(t->e2, origin_cross_e1);

    intersections_t result;
    result.count            = 1;
    result.intersections[0] = intersection_with_uv(ray_t, (void *)t, u, v);
    return result;
}