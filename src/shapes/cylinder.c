#include "../../include/shapes.h"
#include <float.h>
#include <math.h>

static bool check_cap(ray_t ray, double t)
{
    double x = ray.origin.x + t * ray.direction.x;
    double z = ray.origin.z + t * ray.direction.z;
    return (x * x + z * z) <= 1.0;
}

static void intersect_caps(const cylinder_t *cyl, ray_t ray,
                           intersections_t *xs)
{
    if (!cyl->closed || fabs(ray.direction.y) < EPSILON)
    {
        return;
    }

    double t = (cyl->minimum - ray.origin.y) / ray.direction.y;
    if (check_cap(ray, t))
    {
        xs->intersections[xs->count++] = intersection(t, (void *)cyl);
    }

    t = (cyl->maximum - ray.origin.y) / ray.direction.y;
    if (check_cap(ray, t))
    {
        xs->intersections[xs->count++] = intersection(t, (void *)cyl);
    }
}

cylinder_t cylinder(void)
{
    cylinder_t c;
    c.minimum = -DBL_MAX;
    c.maximum = DBL_MAX;
    c.closed  = false;
    shape((shape_t *)&c, SHAPE_CYLINDER);
    return c;
}

intersections_t cylinder_intersect(const cylinder_t *c, ray_t r)
{
    intersections_t result;
    result.count = 0;

    if (c == NULL)
    {
        return result;
    }

    double a = r.direction.x * r.direction.x + r.direction.z * r.direction.z;

    if (fabs(a) >= EPSILON)
    {
        double b = 2.0 * r.origin.x * r.direction.x +
                   2.0 * r.origin.z * r.direction.z;
        double c_coef = r.origin.x * r.origin.x + r.origin.z * r.origin.z - 1.0;

        double disc = b * b - 4.0 * a * c_coef;

        if (disc >= 0.0)
        {
            double sqrt_disc = sqrt(disc);
            double t0        = (-b - sqrt_disc) / (2.0 * a);
            double t1        = (-b + sqrt_disc) / (2.0 * a);

            double y0 = r.origin.y + t0 * r.direction.y;
            if (c->minimum < y0 && y0 < c->maximum)
            {
                result.intersections[result.count++] = intersection(t0, (void *)c);
            }

            double y1 = r.origin.y + t1 * r.direction.y;
            if (c->minimum < y1 && y1 < c->maximum)
            {
                result.intersections[result.count++] = intersection(t1, (void *)c);
            }
        }
    }

    intersect_caps(c, r, &result);

    return result;
}
