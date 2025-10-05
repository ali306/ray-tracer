#include "../../include/shapes.h"
#include <float.h>
#include <math.h>

static bool check_cap(ray_t ray, double t, double y)
{
    double x = ray.origin.x + t * ray.direction.x;
    double z = ray.origin.z + t * ray.direction.z;
    return (x * x + z * z) <= (y * y);
}

static void intersect_caps(const cone_t *cone, ray_t ray, intersections_t *xs)
{
    if (!cone->closed || fabs(ray.direction.y) < EPSILON)
    {
        return;
    }

    double t = (cone->minimum - ray.origin.y) / ray.direction.y;
    if (check_cap(ray, t, cone->minimum))
    {
        xs->intersections[xs->count++] = intersection(t, (void *)cone);
    }

    t = (cone->maximum - ray.origin.y) / ray.direction.y;
    if (check_cap(ray, t, cone->maximum))
    {
        xs->intersections[xs->count++] = intersection(t, (void *)cone);
    }
}

cone_t cone(void)
{
    cone_t c;
    c.minimum = -DBL_MAX;
    c.maximum = DBL_MAX;
    c.closed  = false;
    shape((shape_t *)&c, SHAPE_CONE);
    return c;
}

intersections_t cone_intersect(const cone_t *c, ray_t r)
{
    intersections_t result;
    result.count = 0;

    if (c == NULL)
    {
        return result;
    }

    double a = r.direction.x * r.direction.x - r.direction.y * r.direction.y +
               r.direction.z * r.direction.z;

    double b = 2.0 * r.origin.x * r.direction.x -
               2.0 * r.origin.y * r.direction.y +
               2.0 * r.origin.z * r.direction.z;

    double c_coef = r.origin.x * r.origin.x - r.origin.y * r.origin.y +
                    r.origin.z * r.origin.z;

    if (fabs(a) < EPSILON)
    {
        if (fabs(b) < EPSILON)
        {
            intersect_caps(c, r, &result);
            return result;
        }

        double t = -c_coef / (2.0 * b);
        double y = r.origin.y + t * r.direction.y;
        if (c->minimum < y && y < c->maximum)
        {
            result.intersections[result.count++] = intersection(t, (void *)c);
        }

        intersect_caps(c, r, &result);
        return result;
    }

    double disc = b * b - 4.0 * a * c_coef;

    if (disc < -EPSILON)
    {
        intersect_caps(c, r, &result);
        return result;
    }

    if (disc < 0.0)
    {
        disc = 0.0;
    }

    double sqrt_disc = sqrt(disc);
    double t0        = (-b - sqrt_disc) / (2.0 * a);
    double t1        = (-b + sqrt_disc) / (2.0 * a);

    if (t0 > t1)
    {
        double temp = t0;
        t0          = t1;
        t1          = temp;
    }

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

    intersect_caps(c, r, &result);

    return result;
}
