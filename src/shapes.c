#include "../include/shapes.h"
#include "../include/bounds.h"
#include "../include/patterns.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void shape(shape_t *s, const shape_type_t t)
{
    if (s == NULL)
    {
        return;
    }

    s->type                         = t;
    s->transform                    = IDENTITY;
    s->inverse_transform            = IDENTITY;
    s->transposed_inverse_transform = IDENTITY;
    s->material                     = material();
    s->parent                       = NULL;

    bounding_box_t local_bounds = bounds_of(s);
    s->world_bounds             = local_bounds;
    s->world_bounds_cached      = true;
}

void shape_set_transform(shape_t *s, const matrix_t m)
{
    if (s == NULL)
    {
        return;
    }

    s->transform                      = m;
    s->inverse_transform              = matrix_inverse(m);
    s->transposed_inverse_transform   = matrix_transpose(s->inverse_transform);

    bounding_box_t local_bounds = bounds_of(s);
    s->world_bounds             = bounds_transform(local_bounds, m);
    s->world_bounds_cached      = true;
}

tuple_t shape_normal_at(const shape_t *s, const tuple_t world_point,
                        const intersection_t *hit)
{
    if (s == NULL)
    {
        return vector(0, 1, 0);
    }

    tuple_t object_point = world_to_object(s, world_point);
    tuple_t object_normal;

    switch (s->type)
    {
    case SHAPE_SPHERE:
    {
        object_normal = tuple_subtract(object_point, point(0, 0, 0));
    }
    break;
    case SHAPE_PLANE:
        object_normal = vector(0, 1, 0);
        break;
    case SHAPE_CUBE:
    {
        double abs_x = fabs(object_point.x);
        double abs_y = fabs(object_point.y);
        double abs_z = fabs(object_point.z);

        double max_c = fmax(fmax(abs_x, abs_y), abs_z);

        if (fabs(abs_x - max_c) < EPSILON)
        {
            object_normal = vector(object_point.x, 0, 0);
        }
        else if (fabs(abs_y - max_c) < EPSILON)
        {
            object_normal = vector(0, object_point.y, 0);
        }
        else
        {
            object_normal = vector(0, 0, object_point.z);
        }
    }
    break;
    case SHAPE_CYLINDER:
    {
        cylinder_t *cyl = (cylinder_t *)s;
        double dist     = object_point.x * object_point.x +
                      object_point.z * object_point.z;

        if (dist < 1.0 && object_point.y >= cyl->maximum - EPSILON)
        {
            object_normal = vector(0, 1, 0);
        }
        else if (dist < 1.0 && object_point.y <= cyl->minimum + EPSILON)
        {
            object_normal = vector(0, -1, 0);
        }
        else
        {
            object_normal = vector(object_point.x, 0, object_point.z);
        }
    }
    break;
    case SHAPE_CONE:
    {
        cone_t *cone = (cone_t *)s;
        double dist  = object_point.x * object_point.x +
                      object_point.z * object_point.z;
        double y_squared = object_point.y * object_point.y;

        if (dist < y_squared && object_point.y >= cone->maximum - EPSILON)
        {
            object_normal = vector(0, 1, 0);
        }
        else if (dist < y_squared && object_point.y <= cone->minimum + EPSILON)
        {
            object_normal = vector(0, -1, 0);
        }
        else
        {
            double y = sqrt(dist);
            if (object_point.y > 0.0)
            {
                y = -y;
            }
            object_normal = vector(object_point.x, y, object_point.z);
        }
    }
    break;
    case SHAPE_TRIANGLE:
    {
        triangle_t *tri = (triangle_t *)s;
        object_normal   = tri->normal;
    }
    break;
    case SHAPE_SMOOTH_TRIANGLE:
    {
        smooth_triangle_t *tri = (smooth_triangle_t *)s;
        if (hit != NULL)
        {
            double w      = 1.0 - hit->u - hit->v;
            object_normal = tuple_add(tuple_add(tuple_scale(tri->n2, hit->u),
                                                tuple_scale(tri->n3, hit->v)),
                                      tuple_scale(tri->n1, w));
        }
        else
        {
            object_normal = tri->normal;
        }
    }
    break;
    case SHAPE_GROUP:
        object_normal = vector(0, 1, 0);
        break;
    default:
        object_normal = tuple_normalize(
            vector(object_point.x, object_point.y, object_point.z));
    }

    return normal_to_world(s, object_normal);
}

tuple_t normal_at(const void *shape, const tuple_t world_point,
                  const intersection_t *hit)
{
    if (shape == NULL)
    {
        return vector(0, 1, 0);
    }

    return shape_normal_at((const shape_t *)shape, world_point, hit);
}

__attribute__((hot)) intersections_t shape_intersect(const shape_t *s,
                                                     const ray_t r)
{
    if (__builtin_expect(s == NULL, 0))
    {
        return empty_intersections();
    }

    if (!bounds_intersects(s->world_bounds, r))
    {
        return empty_intersections();
    }

    ray_t local_ray = ray_transform(r, s->inverse_transform);

    switch (s->type)
    {
    case SHAPE_SPHERE:
        return sphere_intersect((sphere_t *)s, local_ray);
    case SHAPE_PLANE:
        return plane_intersect((plane_t *)s, local_ray);
    case SHAPE_CUBE:
        return cube_intersect((cube_t *)s, local_ray);
    case SHAPE_CYLINDER:
        return cylinder_intersect((cylinder_t *)s, local_ray);
    case SHAPE_CONE:
        return cone_intersect((cone_t *)s, local_ray);
    case SHAPE_TRIANGLE:
        return triangle_intersect((triangle_t *)s, local_ray);
    case SHAPE_SMOOTH_TRIANGLE:
        return smooth_triangle_intersect((smooth_triangle_t *)s, local_ray);
    case SHAPE_GROUP:
        return group_intersect((group_t *)s, local_ray);
    case SHAPE_TEST:
        return test_shape_intersect((test_shape_t *)s, local_ray);
    }
    __builtin_unreachable();
}

tuple_t pattern_at_shape(pattern_t p, shape_t o, tuple_t world_point)
{
    tuple_t object_point  = matrix_tmul(o.inverse_transform, world_point);
    tuple_t pattern_point = matrix_tmul(p.inverse_transform, object_point);
    return pattern_at(p, pattern_point);
}

tuple_t world_to_object(const shape_t *shape, tuple_t point)
{
    if (shape == NULL)
    {
        return point;
    }

    if (shape->parent != NULL)
    {
        point = world_to_object((shape_t *)shape->parent, point);
    }

    return matrix_tmul(shape->inverse_transform, point);
}

tuple_t normal_to_world(const shape_t *shape, tuple_t normal)
{
    if (shape == NULL)
    {
        return normal;
    }

    normal   = matrix_tmul(shape->transposed_inverse_transform, normal);
    normal.w = 0;
    normal   = tuple_normalize(normal);

    if (shape->parent != NULL)
    {
        normal = normal_to_world((shape_t *)shape->parent, normal);
    }

    return normal;
}

test_shape_t test_shape(void)
{
    test_shape_t t;
    t.type                         = SHAPE_TEST;
    t.transform                    = IDENTITY;
    t.inverse_transform            = IDENTITY;
    t.transposed_inverse_transform = IDENTITY;
    t.material                     = material();
    t.parent                       = NULL;
    t.has_saved_ray                = false;
    t.world_bounds                 = bounding_box(point(-1, -1, -1), point(1, 1, 1));
    t.world_bounds_cached          = true;
    return t;
}

intersections_t test_shape_intersect(test_shape_t *t, ray_t r)
{
    if (t == NULL)
    {
        return empty_intersections();
    }

    t->saved_ray     = r;
    t->has_saved_ray = true;

    return empty_intersections();
}