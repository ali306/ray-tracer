#include "../include/world.h"
#include <assert.h>

void test_world(void)
{
    {
        world_t w = world();
        assert(w.object_count == 0);
        assert(w.light_count == 0);
        world_free(&w);
    }

    {
        world_t w = world_default();

        assert(w.light_count == 1);
        assert(tuple_equal(w.lights[0].position, point(-10, 10, -10)));
        assert(tuple_equal(w.lights[0].intensity, color(1, 1, 1)));
        assert(w.object_count == 2);
        assert(tuple_equal(w.objects[0].sphere.material.color,
                           color(0.8, 1.0, 0.6)));
        assert(equal(w.objects[0].sphere.material.diffuse, 0.7));
        assert(equal(w.objects[0].sphere.material.specular, 0.2));
        assert(matrix_equal(w.objects[1].sphere.transform,
                            transform_scaling(0.5, 0.5, 0.5)));
        world_free(&w);
    }

    {
        world_t w = world_default();
        ray_t r   = ray(point(0, 0, -5), vector(0, 0, 1));
        intersections_t xs;
        world_intersect(&w, &r, &xs);

        assert(xs.count == 4);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 4.5));
        assert(equal(xs.intersections[2].t, 5.5));
        assert(equal(xs.intersections[3].t, 6.0));
        world_free(&w);
    }

    {
        world_t w            = world_default();
        ray_t r              = ray(point(0, 0, -5), vector(0, 0, 1));
        sphere_t *shape      = &w.objects[0].sphere;
        intersection_t i     = intersection(4, shape);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);
        tuple_t c            = world_shade_hit(&w, &comps, 0);

        assert(tuple_equal(c, color(0.38066, 0.47583, 0.2855)));
        world_free(&w);
    }

    {
        world_t w       = world_default();
        w.lights[0]     = lights_point_light(point(0, 0.25, 0), color(1, 1, 1));
        ray_t r         = ray(point(0, 0, 0), vector(0, 0, 1));
        sphere_t *shape = &w.objects[1].sphere;
        intersection_t i     = intersection(0.5, shape);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);
        tuple_t c            = world_shade_hit(&w, &comps, 0);

        assert(tuple_equal(c, color(0.90498, 0.90498, 0.90498)));
        world_free(&w);
    }

    {
        world_t w = world_default();
        ray_t r   = ray(point(0, 0, -5), vector(0, 1, 0));
        tuple_t c = world_color_at(&w, &r, 0);

        assert(tuple_equal(c, color(0, 0, 0)));
        world_free(&w);
    }

    {
        world_t w = world_default();
        ray_t r   = ray(point(0, 0, -5), vector(0, 0, 1));
        tuple_t c = world_color_at(&w, &r, 0);

        assert(tuple_equal(c, color(0.38066, 0.47583, 0.2855)));
        world_free(&w);
    }

    {
        world_t w               = world_default();
        sphere_t *outer         = &w.objects[0].sphere;
        sphere_t *inner         = &w.objects[1].sphere;
        outer->material.ambient = 1.0;
        inner->material.ambient = 1.0;
        ray_t r                 = ray(point(0, 0, 0.75), vector(0, 0, -1));
        tuple_t c               = world_color_at(&w, &r, 0);

        assert(tuple_equal(c, inner->material.color));
        world_free(&w);
    }

    {
        world_t w              = world_default();
        tuple_t light_position = point(-10, -10, -10);

        tuple_t point1 = point(-10, -10, 10);
        assert(!world_is_shadowed(&w, light_position, point1));

        tuple_t point2 = point(10, 10, 10);
        assert(world_is_shadowed(&w, light_position, point2));

        tuple_t point3 = point(-20, -20, -20);
        assert(!world_is_shadowed(&w, light_position, point3));

        tuple_t point4 = point(-5, -5, -5);
        assert(!world_is_shadowed(&w, light_position, point4));
        world_free(&w);
    }

    {
        world_t w = world();
        world_add_light(&w, lights_point_light(point(0, 0, -10), color(1, 1, 1)));

        sphere_t s1 = sphere();
        sphere_t s2 = sphere();
        sphere_set_transform(&s2, transform_translation(0, 0, 10));

        world_add_shape(&w, s1);
        world_add_shape(&w, s2);

        ray_t r              = ray(point(0, 0, 5), vector(0, 0, 1));
        intersection_t i     = intersection(4, &w.objects[1].sphere);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);
        tuple_t c            = world_shade_hit(&w, &comps, 0);

        assert(tuple_equal(c, color(0.1, 0.1, 0.1)));
        world_free(&w);
    }

    {
        world_t w             = world_default();
        plane_t p             = plane();
        p.material.reflective = 0.5;
        shape_set_transform(&p, transform_translation(0, -1, 0));
        world_add_shape(&w, p);

        ray_t r = ray(point(0, 0, -3), vector(0, -sqrt(2) / 2, sqrt(2) / 2));
        intersection_t i     = intersection(sqrt(2), &p);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);

        tuple_t c = world_reflected_color(&w, &comps, MAX_RECURSION);

        assert(tuple_equal(c, color(0.19032, 0.2379, 0.14274)));
        world_free(&w);
    }

    {
        world_t w             = world_default();
        plane_t p             = plane();
        p.material.reflective = 0.5;
        shape_set_transform(&p, transform_translation(0, -1, 0));
        world_add_shape(&w, p);

        ray_t r = ray(point(0, 0, -3), vector(0, -sqrt(2) / 2, sqrt(2) / 2));
        intersection_t i     = intersection(sqrt(2), &p);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);
        tuple_t c            = world_shade_hit(&w, &comps, MAX_RECURSION);

        assert(tuple_equal(c, color(0.87677, 0.92436, 0.82918)));
        world_free(&w);
    }

    {
        world_t w = world();
        world_add_light(&w, lights_point_light(point(0, 0, 0), color(1, 1, 1)));

        plane_t lower             = plane();
        lower.material.reflective = 1.0;
        shape_set_transform(&lower, transform_translation(0, -1, 0));
        world_add_shape(&w, lower);

        plane_t upper             = plane();
        upper.material.reflective = 1.0;
        shape_set_transform(&upper, transform_translation(0, 1, 0));
        world_add_shape(&w, upper);

        ray_t r = ray(point(0, 0, 0), vector(0, 1, 0));

        tuple_t c = world_color_at(&w, &r, MAX_RECURSION);
        assert(c.x >= 0 && c.y >= 0 && c.z >= 0);
        world_free(&w);
    }

    {
        world_t w             = world_default();
        plane_t p             = plane();
        p.material.reflective = 0.5;
        shape_set_transform(&p, transform_translation(0, -1, 0));
        world_add_shape(&w, p);

        ray_t r = ray(point(0, 0, -3), vector(0, -sqrt(2) / 2, sqrt(2) / 2));
        intersection_t i     = intersection(sqrt(2), &p);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);
        tuple_t c            = world_shade_hit(&w, &comps, MAX_RECURSION);

        assert(tuple_equal(c, color(0.87677, 0.92436, 0.82918)));
        world_free(&w);
    }

    {
        world_t w          = world_default();
        shape_t *shape     = &w.objects[0].sphere;
        ray_t r            = ray(point(0, 0, -5), vector(0, 0, 1));
        intersection_t i1  = intersection(4, shape);
        intersection_t i2  = intersection(6, shape);
        intersections_t xs = intersections(2, i1, i2);
        computations_t comps =
            intersections_prepare_computations(&xs.intersections[0], &r, &xs);
        tuple_t c = world_refracted_color(&w, &comps, 5);

        assert(tuple_equal(c, color(0, 0, 0)));
        world_free(&w);
    }

    {
        world_t w                        = world_default();
        shape_t *shape                   = &w.objects[0].sphere;
        shape->material.transparency     = 1.0;
        shape->material.refractive_index = 1.5;
        ray_t r            = ray(point(0, 0, -5), vector(0, 0, 1));
        intersection_t i1  = intersection(4, shape);
        intersection_t i2  = intersection(6, shape);
        intersections_t xs = intersections(2, i1, i2);
        computations_t comps =
            intersections_prepare_computations(&xs.intersections[0], &r, &xs);
        tuple_t c = world_refracted_color(&w, &comps, 0);

        assert(tuple_equal(c, color(0, 0, 0)));
        world_free(&w);
    }

    {
        world_t w = world_default();

        shape_t *A              = &w.objects[0].sphere;
        A->material.ambient     = 1.0;
        A->material.pattern     = patterns_test();
        A->material.has_pattern = true;

        shape_t *B                   = &w.objects[1].sphere;
        B->material.transparency     = 1.0;
        B->material.refractive_index = 1.5;

        ray_t r            = ray(point(0, 0, 0.1), vector(0, 1, 0));
        intersection_t i1  = intersection(-0.9899, A);
        intersection_t i2  = intersection(-0.4899, B);
        intersection_t i3  = intersection(0.4899, B);
        intersection_t i4  = intersection(0.9899, A);
        intersections_t xs = intersections(4, i1, i2, i3, i4);

        computations_t comps =
            intersections_prepare_computations(&xs.intersections[2], &r, &xs);
        tuple_t c = world_refracted_color(&w, &comps, MAX_RECURSION);
        assert(tuple_equal(c, color(0, 0.99888, 0.04725)));
        world_free(&w);
    }

    {
        world_t w = world_default();

        plane_t floor = plane();
        shape_set_transform(&floor, transform_translation(0, -1, 0));
        floor.material.reflective       = 0.5;
        floor.material.transparency     = 0.5;
        floor.material.refractive_index = 1.5;
        world_add_shape(&w, floor);

        sphere_t ball         = sphere();
        ball.material.color   = color(1.0, 0.0, 0.0);
        ball.material.ambient = 0.5;
        shape_set_transform(&ball, transform_translation(0, -3.5, -0.5));
        world_add_shape(&w, ball);

        ray_t r = ray(point(0, 0, -3), vector(0, -sqrt(2) / 2, sqrt(2) / 2));
        intersection_t i   = intersection(sqrt(2), &floor);
        intersections_t xs = intersections(1, i);

        computations_t comps = intersections_prepare_computations(&i, &r, &xs);
        tuple_t c            = world_shade_hit(&w, &comps, 5);

        assert(tuple_equal(c, color(0.93391, 0.69643, 0.69243)));
        world_free(&w);
    }
}

int main(void)
{
    test_world();
    return 0;
}
