// bounds.c

#include "../include/bounds.h"

bounding_box_t bounding_box_empty(void)
{
    return (bounding_box_t){point(DBL_MAX, DBL_MAX, DBL_MAX),
                            point(-DBL_MAX, -DBL_MAX, -DBL_MAX)};
}

bounding_box_t bounding_box(tuple_t min, tuple_t max)
{
    return (bounding_box_t){min, max};
}

void bounds_add_point(bounding_box_t *box, tuple_t point)
{
    if (box == NULL)
    {
        return;
    }

    box->min.x = point.x < box->min.x ? point.x : box->min.x;
    box->min.y = point.y < box->min.y ? point.y : box->min.y;
    box->min.z = point.z < box->min.z ? point.z : box->min.z;

    box->max.x = point.x > box->max.x ? point.x : box->max.x;
    box->max.y = point.y > box->max.y ? point.y : box->max.y;
    box->max.z = point.z > box->max.z ? point.z : box->max.z;
}

bounding_box_t bounds_of(const void *shape_ptr)
{
    if (shape_ptr == NULL)
    {
        return bounding_box_empty();
    }

    const shape_t *s   = (const shape_t *)shape_ptr;
    bounding_box_t box = bounding_box_empty();

    switch (s->type)
    {
    case SHAPE_SPHERE:
        return (bounding_box_t){point(-1, -1, -1), point(1, 1, 1)};
        break;

    case SHAPE_PLANE:
        return (bounding_box_t){point(-DBL_MAX, 0, -DBL_MAX),
                                point(DBL_MAX, 0, DBL_MAX)};
        break;

    case SHAPE_CUBE:
        return (bounding_box_t){point(-1, -1, -1), point(1, 1, 1)};
        break;

    case SHAPE_CYLINDER:
    {
        cylinder_t *cyl = (cylinder_t *)shape_ptr;
        return (bounding_box_t){point(-1, cyl->minimum, -1),
                                point(1, cyl->maximum, 1)};
    }
    break;

    case SHAPE_CONE:
    {
        cone_t *cone = (cone_t *)shape_ptr;
        double a     = fabs(cone->minimum);
        double b     = fabs(cone->maximum);
        double limit = fmax(a, b);

        return (bounding_box_t){point(-limit, cone->minimum, -limit),
                                point(limit, cone->maximum, limit)};
    }
    break;

    case SHAPE_TEST:
        return (bounding_box_t){point(-1, -1, -1), point(1, 1, 1)};
        break;

    case SHAPE_TRIANGLE:
        return bounds_of_triangle(*(const triangle_t *)shape_ptr);
        break;

    case SHAPE_SMOOTH_TRIANGLE:
        return bounds_of_triangle(*(const triangle_t *)shape_ptr);
        break;

    case SHAPE_GROUP:
        return bounds_of_group(shape_ptr);
        break;

    default:
        return box;
        break;
    }
}

bounding_box_t bounds_of_triangle(triangle_t triangle)
{
    bounding_box_t box = bounding_box_empty();
    bounds_add_point(&box, triangle.p1);
    bounds_add_point(&box, triangle.p2);
    bounds_add_point(&box, triangle.p3);
    return box;
}

void bounds_add_box(bounding_box_t *box1, const bounding_box_t *box2)
{
    if (box1 == NULL || box2 == NULL)
    {
        return;
    }

    bounds_add_point(box1, box2->min);
    bounds_add_point(box1, box2->max);
}

bool bounds_box_contains_point(bounding_box_t box, tuple_t point)
{
    return (point.x >= box.min.x && point.x <= box.max.x) &&
           (point.y >= box.min.y && point.y <= box.max.y) &&
           (point.z >= box.min.z && point.z <= box.max.z);
}

bool bounds_box_contains_box(bounding_box_t box1, bounding_box_t box2)
{
    return bounds_box_contains_point(box1, box2.min) &&
           bounds_box_contains_point(box1, box2.max);
}

bounding_box_t bounds_transform(bounding_box_t bbox, matrix_t matrix)
{
    tuple_t p1        = bbox.min;
    tuple_t p2        = point(bbox.min.x, bbox.min.y, bbox.max.z);
    tuple_t p3        = point(bbox.min.x, bbox.max.y, bbox.min.z);
    tuple_t p4        = point(bbox.min.x, bbox.max.y, bbox.max.z);
    tuple_t p5        = point(bbox.max.x, bbox.min.y, bbox.min.z);
    tuple_t p6        = point(bbox.max.x, bbox.min.y, bbox.max.z);
    tuple_t p7        = point(bbox.max.x, bbox.max.y, bbox.min.z);
    tuple_t p8        = bbox.max;
    tuple_t points[8] = {p1, p2, p3, p4, p5, p6, p7, p8};

    bounding_box_t new_bbox = bounding_box_empty();

    for (int i = 0; i < 8; i++)
    {
        bounds_add_point(&new_bbox, matrix_tmul(matrix, points[i]));
    }
    return new_bbox;
}

bounding_box_t bounds_parent_space_bounds_of(const shape_t *shape)
{
    if (shape == NULL)
    {
        return bounding_box_empty();
    }

    return bounds_transform(bounds_of(shape), shape->transform);
}

bounding_box_t bounds_of_group(const void *group_ptr)
{
    if (group_ptr == NULL)
    {
        return bounding_box_empty();
    }

    group_t *group = (group_t *)group_ptr;

    if (group->bounds_cached)
    {
        return bounding_box(group->cached_bounds_min, group->cached_bounds_max);
    }

    bounding_box_t box = bounding_box_empty();

    for (unsigned i = 0; i < group->child_count; i++)
    {
        if (group->children[i] != NULL)
        {
            bounding_box_t cbox;
            if (group->children[i]->type == SHAPE_GROUP)
            {
                bounding_box_t child_group_box =
                    bounds_of_group(group->children[i]);
                cbox = bounds_transform(child_group_box,
                                        group->children[i]->transform);
            }
            else
            {
                cbox = bounds_parent_space_bounds_of(group->children[i]);
            }
            bounds_add_box(&box, &cbox);
        }
    }

    group->cached_bounds_min = box.min;
    group->cached_bounds_max = box.max;
    group->bounds_cached     = true;

    return box;
}

static void bounds_check_axis(const double origin, const double direction,
                              const double min_bound, const double max_bound,
                              double *tmin, double *tmax)
{
    if (tmin == NULL || tmax == NULL)
    {
        return;
    }

    double tmin_numerator = (min_bound - origin);
    double tmax_numerator = (max_bound - origin);

    if (fabs(direction) >= EPSILON)
    {
        *tmin = tmin_numerator / direction;
        *tmax = tmax_numerator / direction;
    }
    else
    {
        *tmin = tmin_numerator >= 0 ? 1e6 : -1e6;
        *tmax = tmax_numerator >= 0 ? 1e6 : -1e6;
    }

    if (*tmin > *tmax)
    {
        double temp = *tmin;
        *tmin       = *tmax;
        *tmax       = temp;
    }
}

bool bounds_intersects(bounding_box_t box, ray_t ray)
{
    double xtmin, xtmax, ytmin, ytmax, ztmin, ztmax;

    bounds_check_axis(ray.origin.x, ray.direction.x, box.min.x, box.max.x,
                      &xtmin, &xtmax);
    bounds_check_axis(ray.origin.y, ray.direction.y, box.min.y, box.max.y,
                      &ytmin, &ytmax);
    bounds_check_axis(ray.origin.z, ray.direction.z, box.min.z, box.max.z,
                      &ztmin, &ztmax);

    double tmin = fmax(fmax(xtmin, ytmin), ztmin);
    double tmax = fmin(fmin(xtmax, ytmax), ztmax);

    return tmin <= tmax;
}

void split_bounds(bounding_box_t box, bounding_box_t *left,
                  bounding_box_t *right)
{
    if (left == NULL || right == NULL)
    {
        return;
    }

    double dx = box.max.x - box.min.x;
    double dy = box.max.y - box.min.y;
    double dz = box.max.z - box.min.z;

    double greatest = fmax(fmax(dx, dy), dz);

    double x0 = box.min.x, y0 = box.min.y, z0 = box.min.z;
    double x1 = box.max.x, y1 = box.max.y, z1 = box.max.z;

    if (fabs(greatest - dx) < EPSILON)
    {
        x0 = x1 = box.min.x + dx / 2.0;
    }
    else if (fabs(greatest - dy) < EPSILON)
    {
        y0 = y1 = box.min.y + dy / 2.0;
    }
    else
    {
        z0 = z1 = box.min.z + dz / 2.0;
    }

    tuple_t mid_min = point(x0, y0, z0);
    tuple_t mid_max = point(x1, y1, z1);

    *left  = bounding_box(box.min, mid_max);
    *right = bounding_box(mid_min, box.max);
}