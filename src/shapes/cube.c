#include "../../include/shapes.h"
#include <math.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

cube_t cube(void)
{
    cube_t c;
    shape(&c, SHAPE_CUBE);
    return c;
}

intersections_t cube_intersect(const cube_t *c, ray_t r)
{
    if (c == NULL)
    {
        return empty_intersections();
    }

    double xtmin, xtmax, ytmin, ytmax, ztmin, ztmax;

    check_axis(r.origin.x, r.direction.x, &xtmin, &xtmax);
    check_axis(r.origin.y, r.direction.y, &ytmin, &ytmax);
    check_axis(r.origin.z, r.direction.z, &ztmin, &ztmax);

    double tmin = MAX(MAX(xtmin, ytmin), ztmin);
    double tmax = MIN(MIN(xtmax, ytmax), ztmax);

    if (tmin > tmax)
    {
        return empty_intersections();
    }

    intersections_t result;
    result.count            = 2;
    result.intersections[0] = intersection(tmin, (void *)c);
    result.intersections[1] = intersection(tmax, (void *)c);
    return result;
}