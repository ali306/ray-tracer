// scene_reflect_refract.c
// Recreation of the reflect-refract scene from "The Ray Tracer Challenge"
// chapter 11

#include "../../include/lights.h"
#include "../../include/patterns.h"
#include "../../include/scenes.h"

bool scene_reflect_refract(void)
{
    world_t w = world();

    camera_t c              = camera(400 * 4, 200 * 4, 1.152);
    matrix_t view_transform = transform_view(point(-2.6, 1.5, -3.9), // from
                                             point(-0.6, 1.0, -0.8), // to
                                             vector(0, 1, 0)         // up
    );
    camera_set_transform(&c, view_transform);

    world_add_light(&w,
                    lights_point_light(point(-4.9, 4.9, -1), color(1, 1, 1)));

    material_t wall_material  = material();
    wall_material.has_pattern = true;
    wall_material.pattern =
        patterns_stripe(color(0.45, 0.45, 0.45), color(0.55, 0.55, 0.55));
    matrix_t wall_pattern_transform = matrix_mul(
        transform_scaling(0.25, 0.25, 0.25), transform_rotation_y(1.5708));
    patterns_set_transform(&wall_material.pattern, wall_pattern_transform);
    wall_material.ambient    = 0.0;
    wall_material.diffuse    = 0.4;
    wall_material.specular   = 0.0;
    wall_material.reflective = 0.3;

    {
        plane_t floor = plane();
        shape_set_transform(&floor, transform_rotation_y(0.31415));
        floor.material.has_pattern = true;
        floor.material.pattern =
            patterns_checker(color(0.35, 0.35, 0.35), color(0.65, 0.65, 0.65));
        floor.material.specular   = 0.0;
        floor.material.reflective = 0.4;
        world_add_shape(&w, floor);
    }

    {
        plane_t ceiling = plane();
        shape_set_transform(&ceiling, transform_translation(0, 5, 0));
        ceiling.material.color    = color(0.8, 0.8, 0.8);
        ceiling.material.ambient  = 0.3;
        ceiling.material.specular = 0.0;
        world_add_shape(&w, ceiling);
    }

    {
        plane_t west_wall = plane();
        matrix_t west_transform =
            matrix_mul(transform_translation(-5, 0, 0),
                       matrix_mul(transform_rotation_z(1.5708),
                                  transform_rotation_y(1.5708)));
        shape_set_transform(&west_wall, west_transform);
        west_wall.material = wall_material;
        world_add_shape(&w, west_wall);
    }

    {
        plane_t east_wall = plane();
        matrix_t east_transform =
            matrix_mul(transform_translation(5, 0, 0),
                       matrix_mul(transform_rotation_z(1.5708),
                                  transform_rotation_y(1.5708)));
        shape_set_transform(&east_wall, east_transform);
        east_wall.material = wall_material;
        world_add_shape(&w, east_wall);
    }

    {
        plane_t north_wall       = plane();
        matrix_t north_transform = matrix_mul(transform_translation(0, 0, 5),
                                              transform_rotation_x(1.5708));
        shape_set_transform(&north_wall, north_transform);
        north_wall.material = wall_material;
        world_add_shape(&w, north_wall);
    }

    {
        plane_t south_wall       = plane();
        matrix_t south_transform = matrix_mul(transform_translation(0, 0, -5),
                                              transform_rotation_x(1.5708));
        shape_set_transform(&south_wall, south_transform);
        south_wall.material = wall_material;
        world_add_shape(&w, south_wall);
    }

    // Background spheres
    {
        sphere_t s1           = sphere();
        matrix_t s1_transform = matrix_mul(transform_translation(4.6, 0.4, 1.0),
                                           transform_scaling(0.4, 0.4, 0.4));
        shape_set_transform(&s1, s1_transform);
        s1.material.color     = color(0.8, 0.5, 0.3);
        s1.material.shininess = 50.0;
        world_add_shape(&w, s1);

        sphere_t s2           = sphere();
        matrix_t s2_transform = matrix_mul(transform_translation(4.7, 0.3, 0.4),
                                           transform_scaling(0.3, 0.3, 0.3));
        shape_set_transform(&s2, s2_transform);
        s2.material.color     = color(0.9, 0.4, 0.5);
        s2.material.shininess = 50.0;
        world_add_shape(&w, s2);

        sphere_t s3 = sphere();
        matrix_t s3_transform =
            matrix_mul(transform_translation(-1.0, 0.5, 4.5),
                       transform_scaling(0.5, 0.5, 0.5));
        shape_set_transform(&s3, s3_transform);
        s3.material.color     = color(0.4, 0.9, 0.6);
        s3.material.shininess = 50.0;
        world_add_shape(&w, s3);

        sphere_t s4 = sphere();
        matrix_t s4_transform =
            matrix_mul(transform_translation(-1.7, 0.3, 4.7),
                       transform_scaling(0.3, 0.3, 0.3));
        shape_set_transform(&s4, s4_transform);
        s4.material.color     = color(0.4, 0.6, 0.9);
        s4.material.shininess = 50.0;
        world_add_shape(&w, s4);
    }

    {
        sphere_t red_sphere = sphere();
        shape_set_transform(&red_sphere, transform_translation(-0.6, 1.0, 0.6));
        red_sphere.material.color     = color(1.0, 0.3, 0.2);
        red_sphere.material.specular  = 0.4;
        red_sphere.material.shininess = 5.0;
        world_add_shape(&w, red_sphere);

        sphere_t blue_glass = glass_sphere();
        matrix_t blue_transform =
            matrix_mul(transform_translation(0.6, 0.7, -0.6),
                       transform_scaling(0.7, 0.7, 0.7));
        shape_set_transform(&blue_glass, blue_transform);
        blue_glass.material.color            = color(0.0, 0.0, 0.2);
        blue_glass.material.ambient          = 0.0;
        blue_glass.material.diffuse          = 0.4;
        blue_glass.material.specular         = 0.9;
        blue_glass.material.shininess        = 300.0;
        blue_glass.material.reflective       = 0.9;
        blue_glass.material.transparency     = 0.9;
        blue_glass.material.refractive_index = 1.5;
        world_add_shape(&w, blue_glass);

        sphere_t green_glass = glass_sphere();
        matrix_t green_transform =
            matrix_mul(transform_translation(-0.7, 0.5, -0.8),
                       transform_scaling(0.5, 0.5, 0.5));
        shape_set_transform(&green_glass, green_transform);
        green_glass.material.color            = color(0.0, 0.2, 0.0);
        green_glass.material.ambient          = 0.0;
        green_glass.material.diffuse          = 0.4;
        green_glass.material.specular         = 0.9;
        green_glass.material.shininess        = 300.0;
        green_glass.material.reflective       = 0.9;
        green_glass.material.transparency     = 0.9;
        green_glass.material.refractive_index = 1.5;
        world_add_shape(&w, green_glass);
    }

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render reflect-refract scene\n");
        world_free(&w);
        return false;
    }

    if (!canvas_save(image, "../renders/scene_reflect_refract.ppm"))
    {
        printf("Failed to save reflect-refract scene\n");
        canvas_free(image);
        world_free(&w);
        return false;
    }

    canvas_free(image);
    world_free(&w);
    return true;
}