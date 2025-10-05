#include "../../include/shapes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

sphere_t sphere(void)
{
    sphere_t s;
    shape(&s, SHAPE_SPHERE);
    return s;
}

intersections_t sphere_intersect(const sphere_t *s, const ray_t r)
{
    if (s == NULL)
    {
        return empty_intersections();
    }

    tuple_t sphere_to_ray = tuple_subtract(r.origin, point(0, 0, 0));

    double a = tuple_dot(r.direction, r.direction);

    double b = 2.0 * tuple_dot(r.direction, sphere_to_ray);
    double c = tuple_dot(sphere_to_ray, sphere_to_ray) - 1.0;

    if (fabs(a) < EPSILON)
    {
        return empty_intersections();
    }

    double discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0)
    {
        return empty_intersections();
    }

    double sqrt_d = sqrt(discriminant);
    double inv_2a = 0.5 / a;

    double t1 = (-b - sqrt_d) * inv_2a;
    double t2 = (-b + sqrt_d) * inv_2a;

    intersections_t result;
    result.count            = 2;
    result.intersections[0] = intersection(t1, (void *)s);
    result.intersections[1] = intersection(t2, (void *)s);
    return result;
}

void sphere_set_transform(sphere_t *s, matrix_t m)
{
    if (s == NULL)
    {
        return;
    }
    shape_set_transform((shape_t *)s, m);
}

sphere_t glass_sphere(void)
{
    sphere_t s                  = sphere();
    s.material.transparency     = 1.0;
    s.material.refractive_index = 1.5;

    return s;
}