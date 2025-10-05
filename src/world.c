// world.c

#include "../include/world.h"
#include "../include/dynamic_array.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

world_t world(void)
{
    world_t w         = {0};
    w.object_capacity = 25000;
    w.objects         = malloc(w.object_capacity * sizeof(object_t));
    if (!w.objects)
    {
        fprintf(stderr, "Error: Failed to allocate memory for world objects\n");
        exit(EXIT_FAILURE);
    }
    w.object_count = 0;

    w.light_capacity = 16;
    w.lights         = malloc(w.light_capacity * sizeof(light_t));
    if (!w.lights)
    {
        fprintf(stderr, "Error: Failed to allocate memory for world lights\n");
        free(w.objects);
        exit(EXIT_FAILURE);
    }
    w.light_count = 0;

    return w;
}

world_t world_default(void)
{
    world_t w = world();
    world_add_light(&w, lights_point_light(point(-10, 10, -10), color(1, 1, 1)));

    sphere_t s1          = sphere();
    s1.material.color    = color(0.8, 1.0, 0.6);
    s1.material.diffuse  = 0.7;
    s1.material.specular = 0.2;
    world_add_shape(&w, s1);

    sphere_t s2 = sphere();
    shape_set_transform(&s2, transform_scaling(0.5, 0.5, 0.5));
    world_add_shape(&w, s2);

    return w;
}

void world_free(world_t *w)
{
    if (w)
    {
        if (w->objects)
        {
            for (unsigned i = 0; i < w->object_count; i++)
            {
                if (w->objects[i].shape.type == SHAPE_GROUP)
                {
                    group_t *g = &w->objects[i].group;
                    if (g->children != NULL)
                    {
                        for (unsigned j = 0; j < g->child_count; j++)
                        {
                            if (g->children[j] != NULL)
                            {
                                if (g->children[j]->type == SHAPE_GROUP)
                                {
                                    group_free((group_t *)g->children[j]);
                                }
                                else
                                {
                                    free(g->children[j]);
                                }
                            }
                        }
                        free(g->children);
                        g->children = NULL;
                    }
                }
            }

            free(w->objects);
            w->objects         = NULL;
            w->object_count    = 0;
            w->object_capacity = 0;
        }

        if (w->lights)
        {
            free(w->lights);
            w->lights        = NULL;
            w->light_count   = 0;
            w->light_capacity = 0;
        }
    }
}

static bool world_ensure_capacity(world_t *w)
{
    if (!w || !w->objects)
    {
        return false;
    }

    if (w->object_count >= w->object_capacity &&
        w->object_capacity >= MAX_NUM_OBJECTS)
    {
        printf("Warning: Reached maximum object limit (%d), cannot add more "
               "objects\n",
               MAX_NUM_OBJECTS);
        return false;
    }

    DYN_ARRAY_ENSURE_CAPACITY_IMPL(w->objects, w->object_count,
                                   w->object_capacity, object_t,
                                   MAX_NUM_OBJECTS);
    return true;
}

void world_add_shape(world_t *w, shape_t s)
{
    if (!world_ensure_capacity(w))
    {
        return;
    }
    w->objects[w->object_count].shape = s;
    w->object_count++;
}

void world_add_cylinder(world_t *w, cylinder_t c)
{
    if (!world_ensure_capacity(w))
    {
        return;
    }
    w->objects[w->object_count].cylinder = c;
    w->object_count++;
}

void world_add_triangle(world_t *w, triangle_t t)
{
    if (!world_ensure_capacity(w))
    {
        return;
    }
    w->objects[w->object_count].triangle = t;
    w->object_count++;
}

void world_add_smooth_triangle(world_t *w, smooth_triangle_t t)
{
    if (!world_ensure_capacity(w))
    {
        return;
    }
    w->objects[w->object_count].smooth_triangle = t;
    w->object_count++;
}

void world_add_group(world_t *w, group_t *g)
{
    if (!world_ensure_capacity(w))
    {
        group_free(g);
        return;
    }
    w->objects[w->object_count].group = *g;
    group_t *copied_group             = &w->objects[w->object_count].group;

    for (unsigned i = 0; i < copied_group->child_count; i++)
    {
        if (copied_group->children[i] != NULL)
        {
            copied_group->children[i]->parent = copied_group;
        }
    }

    w->object_count++;
    free(g);
}

void world_add_light(world_t *w, light_t light)
{
    if (!w || !w->lights)
    {
        return;
    }

    if (w->light_count >= w->light_capacity)
    {
        unsigned new_capacity = w->light_capacity * 2;
        light_t *new_lights =
            realloc(w->lights, new_capacity * sizeof(light_t));
        if (!new_lights)
        {
            fprintf(stderr, "Error: Failed to reallocate memory for lights\n");
            return;
        }
        w->lights        = new_lights;
        w->light_capacity = new_capacity;
    }

    w->lights[w->light_count] = light;
    w->light_count++;
}

void world_intersect(const world_t *w, const ray_t *r, intersections_t *out)
{
    if (w == NULL || r == NULL || out == NULL)
    {
        if (out != NULL)
        {
            out->count = 0;
        }
        return;
    }

    out->count = 0;

    for (size_t i = 0; i < w->object_count; i++)
    {
        shape_t *shape;

        switch (w->objects[i].shape.type)
        {
        case SHAPE_SPHERE:
            shape = (shape_t *)&w->objects[i].sphere;
            break;
        case SHAPE_PLANE:
            shape = (shape_t *)&w->objects[i].plane;
            break;
        case SHAPE_CUBE:
            shape = (shape_t *)&w->objects[i].cube;
            break;
        case SHAPE_TRIANGLE:
            shape = (shape_t *)&w->objects[i].triangle;
            break;
        case SHAPE_SMOOTH_TRIANGLE:
            shape = (shape_t *)&w->objects[i].smooth_triangle;
            break;
        case SHAPE_GROUP:
            shape = (shape_t *)&w->objects[i].group;
            break;
        default:
            shape = &w->objects[i].shape;
            break;
        }
        intersections_t xs = shape_intersect(shape, *r);

        if (xs.count > 0)
        {
            int space_left = MAX_INTERSECTIONS - out->count;
            int to_copy    = 0;

            if (space_left > 0)
            {
                to_copy = (xs.count <= space_left) ? xs.count : space_left;
                memcpy(&out->intersections[out->count], xs.intersections,
                       (size_t)to_copy * sizeof(intersection_t));
                out->count += to_copy;
            }

            if (out->count == MAX_INTERSECTIONS)
            {
                intersections_sort(out);

                for (int j = to_copy; j < xs.count; j++)
                {
                    if (xs.intersections[j].t <
                        out->intersections[MAX_INTERSECTIONS - 1].t)
                    {
                        out->intersections[MAX_INTERSECTIONS - 1] =
                            xs.intersections[j];
                        intersections_sort(out);
                    }
                }
            }
        }
    }

    if (out->count > 1)
    {
        intersections_sort(out);
    }
}

tuple_t world_shade_hit(const world_t *w, const computations_t *c,
                        const unsigned remaining)
{
    if (w == NULL || c == NULL)
    {
        return color(0, 0, 0);
    }

    shape_t *o      = (shape_t *)c->object;
    tuple_t surface = color(0, 0, 0);

    // Sum contributions from all lights
    for (unsigned i = 0; i < w->light_count; i++)
    {
        double intensity =
            lights_intensity_at((light_t *)&w->lights[i], c->over_point, w);

        tuple_t light_contribution =
            materials_lighting(&o->material, o, &w->lights[i], c->over_point,
                               c->eyev, c->normalv, intensity);

        surface = tuple_add(surface, light_contribution);
    }

    tuple_t reflected = world_reflected_color(w, c, remaining);
    tuple_t refracted = world_refracted_color(w, c, remaining);

    material_t m = o->material;

    if (m.reflective > 0 && m.transparency > 0)
    {
        double reflectance = intersections_shlick(c);
        return tuple_add(surface,
                         tuple_add(tuple_scale(reflected, reflectance),
                                   tuple_scale(refracted, 1 - reflectance)));
    }

    return tuple_add(tuple_add(surface, reflected), refracted);
}

tuple_t world_color_at(const world_t *w, const ray_t *r,
                       const unsigned remaining)
{
    if (w == NULL || r == NULL)
    {
        return color(0, 0, 0);
    }

    intersections_t xs;
    world_intersect(w, r, &xs);
    if (intersections_hit(&xs) == NULL)
    {
        return color(0, 0, 0);
    }
    intersection_t hit   = *intersections_hit(&xs);
    computations_t comps = intersections_prepare_computations(&hit, r, &xs);
    return world_shade_hit(w, &comps, remaining);
}

bool world_is_shadowed(const world_t *w, const tuple_t light_position,
                       const tuple_t p)
{
    if (w == NULL)
    {
        return false;
    }

    tuple_t v         = tuple_subtract(light_position, p);
    double distance   = tuple_magnitude(v);
    tuple_t direction = tuple_normalize(v);

    if (distance < EPSILON)
    {
        return false;
    }

    tuple_t offset_point = tuple_add(p, tuple_scale(direction, EPSILON));
    ray_t r              = ray(offset_point, direction);

    intersections_t xs;

    world_intersect(w, &r, &xs);

    intersection_t *h = intersections_hit(&xs);

    if (h != NULL && h->t < distance)
    {
        shape_t *hit_shape = (shape_t *)h->object;
        return hit_shape->material.casts_shadow;
    }

    return false;
}

tuple_t world_reflected_color(const world_t *w, const computations_t *c,
                              const unsigned remaining)
{
    if (w == NULL || c == NULL)
    {
        return color(0, 0, 0);
    }

    shape_t *o = (shape_t *)c->object;

    if (remaining == 0 || o->material.reflective < MIN_RAY_CONTRIBUTION)
    {
        return color(0, 0, 0);
    }

    ray_t reflect_ray = ray(c->over_point, c->reflectv);
    tuple_t color     = world_color_at(w, &reflect_ray, remaining - 1);

    return tuple_scale(color, o->material.reflective);
}

tuple_t world_refracted_color(const world_t *w, const computations_t *c,
                              const unsigned remaining)
{
    if (w == NULL || c == NULL)
    {
        return color(0, 0, 0);
    }

    shape_t *o = (shape_t *)c->object;

    double n_ratio = c->n1 / c->n2;
    double cos_i   = tuple_dot(c->eyev, c->normalv);
    double sin2_t  = (n_ratio * n_ratio) * (1 - cos_i * cos_i);

    if (remaining == 0 || o->material.transparency < MIN_RAY_CONTRIBUTION ||
        sin2_t > 1)
    {
        return color(0, 0, 0);
    }

    double cos_t = sqrt(1.0 - sin2_t);
    tuple_t direction =
        tuple_subtract(tuple_scale(c->normalv, n_ratio * cos_i - cos_t),
                       tuple_scale(c->eyev, n_ratio));

    ray_t refract_ray = ray(c->under_point, direction);

    return tuple_scale(world_color_at(w, &refract_ray, remaining - 1),
                       o->material.transparency);
}
