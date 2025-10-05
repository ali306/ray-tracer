// intersections.c

#include "../include/intersections.h"
#include "../include/shapes.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

intersection_t intersection(double t, void *o)
{
    intersection_t i = {0};
    i.t              = t;
    i.object         = o;
    return i;
}

intersection_t intersection_with_uv(double t, void *o, double u, double v)
{
    intersection_t i = {0};
    i.t              = t;
    i.object         = o;
    i.u              = u;
    i.v              = v;
    return i;
}

intersections_t intersections(int count, ...)
{
    va_list args;
    va_start(args, count);

    intersections_t result;
    result.count = count;

    for (int i = 0; i < count; i++)
    {
        result.intersections[i] = va_arg(args, intersection_t);
    }

    va_end(args);

    return result;
}

intersection_t *intersections_hit(const intersections_t *xs)
{
    if (xs == NULL)
    {
        return NULL;
    }

    intersection_t *hit = NULL;

    for (int i = 0; i < xs->count; i++)
    {
        if (xs->intersections[i].t >= 0)
        {
            if (hit == NULL || xs->intersections[i].t < hit->t)
            {
                hit = (intersection_t *)&xs->intersections[i];
            }
        }
    }

    return hit;
}

computations_t intersections_prepare_computations(const intersection_t *i,
                                                  const ray_t *r,
                                                  const intersections_t *xs)
{
    computations_t comps = {0};

    if (i == NULL || r == NULL)
    {
        return comps;
    }

    comps.t      = i->t;
    comps.object = i->object;
    comps.point  = ray_position(*r, comps.t);
    comps.eyev   = tuple_negate(r->direction);
    comps.normalv =
        shape_normal_at((const shape_t *)comps.object, comps.point, i);
    comps.reflectv = tuple_reflect(r->direction, comps.normalv);

    if (tuple_dot(comps.normalv, comps.eyev) < 0)
    {
        comps.inside  = true;
        comps.normalv = tuple_negate(comps.normalv);
    }
    else
    {
        comps.inside = false;
    }

    comps.over_point =
        tuple_add(comps.point, tuple_scale(comps.normalv, EPSILON));
    comps.under_point =
        tuple_subtract(comps.point, tuple_scale(comps.normalv, EPSILON));

    comps.n1 = 1.0;
    comps.n2 = 1.0;

    if (xs != NULL)
    {
        int containers_count = 0;
        shape_t *containers[MAX_INTERSECTIONS];

        for (int j = 0; j < xs->count; j++)
        {

            bool is_hit = (equal(xs->intersections[j].t, i->t) &&
                           xs->intersections[j].object == i->object);

            if (is_hit)
            {
                if (containers_count == 0)
                {
                    comps.n1 = 1.0;
                }
                else
                {

                    shape_t *containing_object =
                        containers[containers_count - 1];
                    comps.n1 = containing_object->material.refractive_index;
                }
            }

            int object_index = -1;
            for (int k = 0; k < containers_count; k++)
            {
                if (containers[k] == (shape_t *)xs->intersections[j].object)
                {
                    object_index = k;
                    break;
                }
            }

            if (object_index >= 0)
            {
                if (object_index < containers_count - 1)
                {
                    containers[object_index] = containers[containers_count - 1];
                }
                containers_count--;
            }
            else
            {

                containers[containers_count] =
                    (shape_t *)xs->intersections[j].object;
                containers_count++;
            }

            if (is_hit)
            {
                if (containers_count == 0)
                {
                    comps.n2 = 1.0;
                }
                else
                {

                    shape_t *containing_object =
                        containers[containers_count - 1];
                    comps.n2 = containing_object->material.refractive_index;
                }
                break;
            }
        }
    }

    return comps;
}

double intersections_shlick(const computations_t *c)
{
    if (c == NULL)
    {
        return 0.0;
    }

    double cos = tuple_dot(c->eyev, c->normalv);

    if (c->n1 > c->n2)
    {
        double n      = c->n1 / c->n2;
        double sin2_t = (n * n) * (1.0 - cos * cos);

        if (sin2_t > 1.0)
        {
            return 1.0;
        }

        double cos_t = sqrt(1.0 - sin2_t);
        cos          = cos_t;
    }

    double r0 = ((c->n1 - c->n2) / (c->n1 + c->n2)) *
                ((c->n1 - c->n2) / (c->n1 + c->n2));

    return r0 + (1 - r0) *
                    ((1 - cos) * (1 - cos) * (1 - cos) * (1 - cos) * (1 - cos));
}

static int compare_intersections(const void *a, const void *b)
{
    const intersection_t *ia = (const intersection_t *)a;
    const intersection_t *ib = (const intersection_t *)b;

    if (ia->t < ib->t)
        return -1;
    if (ia->t > ib->t)
        return 1;
    return 0;
}

static void insertion_sort_intersections(intersection_t *xs, int count)
{
    for (int i = 1; i < count; i++)
    {
        intersection_t key = xs[i];
        int j              = i - 1;

        while (j >= 0 && xs[j].t > key.t)
        {
            xs[j + 1] = xs[j];
            j--;
        }
        xs[j + 1] = key;
    }
}

void intersections_sort(intersections_t *xs)
{
    if (xs == NULL || xs->count <= 1)
    {
        return;
    }

    if (xs->count <= 20)
    {
        insertion_sort_intersections(xs->intersections, xs->count);
    }
    else
    {
        qsort(xs->intersections, (size_t)xs->count, sizeof(intersection_t),
              compare_intersections);
    }
}
