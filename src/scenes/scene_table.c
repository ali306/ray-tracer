// scene_table.c
// Recreation of the table scene from "The Ray Tracer Challenge" chapter 12

#include "../../include/lights.h"
#include "../../include/patterns.h"
#include "../../include/scenes.h"

bool scene_table(void)
{
    world_t w = world();

    camera_t c              = camera(400 * 4, 200 * 4, 0.785);
    matrix_t view_transform = transform_view(
        point(8.0, 6.0, -8.0), point(0.0, 3.0, 0.0), vector(0, 1, 0));
    camera_set_transform(&c, view_transform);

    world_add_light(&w,
                    lights_point_light(point(0.0, 6.9, -5.0), color(1.0, 1.0, 0.9)));

    {
        cube_t floor       = cube();
        matrix_t transform = matrix_mul(transform_translation(0, -0.1, 0),
                                        transform_scaling(20, 0.1, 20));
        shape_set_transform(&floor, transform);

        floor.material.has_pattern = true;
        floor.material.pattern =
            patterns_checker(color(0.25, 0.25, 0.25), color(0.0, 0.0, 0.0));
        patterns_set_transform(&floor.material.pattern,
                               transform_scaling(0.07, 0.07, 0.07));

        floor.material.ambient    = 0.25;
        floor.material.diffuse    = 0.7;
        floor.material.specular   = 0.9;
        floor.material.shininess  = 300.0;
        floor.material.reflective = 0.1;

        world_add_shape(&w, floor);
    }

    {
        cube_t walls = cube();
        shape_set_transform(&walls, transform_scaling(10, 10, 10));

        walls.material.has_pattern = true;
        walls.material.pattern     = patterns_checker(
            color(0.4863, 0.3765, 0.2941), color(0.3725, 0.2902, 0.2275));
        patterns_set_transform(&walls.material.pattern,
                               transform_scaling(0.05, 20.0, 0.05));

        walls.material.ambient    = 0.1;
        walls.material.diffuse    = 0.7;
        walls.material.specular   = 0.9;
        walls.material.shininess  = 300.0;
        walls.material.reflective = 0.1;

        world_add_shape(&w, walls);
    }

    {
        cube_t table_top   = cube();
        matrix_t transform = matrix_mul(transform_translation(0, 3.1, 0),
                                        transform_scaling(3, 0.1, 2));
        shape_set_transform(&table_top, transform);

        table_top.material.has_pattern = true;
        table_top.material.pattern     = patterns_stripe(
            color(0.5529, 0.4235, 0.3255), color(0.6588, 0.5098, 0.4000));
        matrix_t pattern_transform = matrix_mul(
            transform_scaling(0.05, 0.05, 0.05), transform_rotation_y(0.1));
        patterns_set_transform(&table_top.material.pattern, pattern_transform);

        table_top.material.ambient    = 0.1;
        table_top.material.diffuse    = 0.7;
        table_top.material.specular   = 0.9;
        table_top.material.shininess  = 300.0;
        table_top.material.reflective = 0.2;

        world_add_shape(&w, table_top);
    }

    material_t leg_material = material();
    leg_material.color      = color(0.5529, 0.4235, 0.3255);
    leg_material.ambient    = 0.2;
    leg_material.diffuse    = 0.7;

    {
        cube_t leg1        = cube();
        matrix_t transform = matrix_mul(transform_translation(2.7, 1.5, -1.7),
                                        transform_scaling(0.1, 1.5, 0.1));
        shape_set_transform(&leg1, transform);
        leg1.material = leg_material;
        world_add_shape(&w, leg1);
    }

    {
        cube_t leg2        = cube();
        matrix_t transform = matrix_mul(transform_translation(2.7, 1.5, 1.7),
                                        transform_scaling(0.1, 1.5, 0.1));
        shape_set_transform(&leg2, transform);
        leg2.material = leg_material;
        world_add_shape(&w, leg2);
    }

    {
        cube_t leg3        = cube();
        matrix_t transform = matrix_mul(transform_translation(-2.7, 1.5, -1.7),
                                        transform_scaling(0.1, 1.5, 0.1));
        shape_set_transform(&leg3, transform);
        leg3.material = leg_material;
        world_add_shape(&w, leg3);
    }

    {
        cube_t leg4        = cube();
        matrix_t transform = matrix_mul(transform_translation(-2.7, 1.5, 1.7),
                                        transform_scaling(0.1, 1.5, 0.1));
        shape_set_transform(&leg4, transform);
        leg4.material = leg_material;
        world_add_shape(&w, leg4);
    }

    {
        cube_t glass_cube = cube();
        matrix_t transform =
            matrix_mul(transform_translation(0, 3.45001, 0),
                       matrix_mul(transform_rotation_y(0.2),
                                  transform_scaling(0.25, 0.25, 0.25)));
        shape_set_transform(&glass_cube, transform);

        glass_cube.material.color            = color(1.0, 1.0, 0.8);
        glass_cube.material.ambient          = 0.0;
        glass_cube.material.diffuse          = 0.3;
        glass_cube.material.specular         = 0.9;
        glass_cube.material.shininess        = 300.0;
        glass_cube.material.reflective       = 0.7;
        glass_cube.material.transparency     = 0.7;
        glass_cube.material.refractive_index = 1.5;

        world_add_shape(&w, glass_cube);
    }

    {
        cube_t little1 = cube();
        matrix_t transform =
            matrix_mul(transform_translation(1.0, 3.35, -0.9),
                       matrix_mul(transform_rotation_y(-0.4),
                                  transform_scaling(0.15, 0.15, 0.15)));
        shape_set_transform(&little1, transform);

        little1.material.color      = color(1.0, 0.5, 0.5);
        little1.material.reflective = 0.6;
        little1.material.diffuse    = 0.4;

        world_add_shape(&w, little1);
    }

    {
        cube_t little2 = cube();
        matrix_t transform =
            matrix_mul(transform_translation(-1.5, 3.27, 0.3),
                       matrix_mul(transform_rotation_y(0.4),
                                  transform_scaling(0.15, 0.07, 0.15)));
        shape_set_transform(&little2, transform);

        little2.material.color = color(1.0, 1.0, 0.5);

        world_add_shape(&w, little2);
    }

    {
        cube_t little3 = cube();
        matrix_t transform =
            matrix_mul(transform_translation(0.0, 3.25, 1.0),
                       matrix_mul(transform_rotation_y(0.4),
                                  transform_scaling(0.2, 0.05, 0.05)));
        shape_set_transform(&little3, transform);

        little3.material.color = color(0.5, 1.0, 0.5);

        world_add_shape(&w, little3);
    }

    {
        cube_t little4 = cube();
        matrix_t transform =
            matrix_mul(transform_translation(-0.6, 3.4, -1.0),
                       matrix_mul(transform_rotation_y(0.8),
                                  transform_scaling(0.05, 0.2, 0.05)));
        shape_set_transform(&little4, transform);

        little4.material.color = color(0.5, 0.5, 1.0);

        world_add_shape(&w, little4);
    }

    {
        cube_t little5 = cube();
        matrix_t transform =
            matrix_mul(transform_translation(2.0, 3.4, 1.0),
                       matrix_mul(transform_rotation_y(0.8),
                                  transform_scaling(0.05, 0.2, 0.05)));
        shape_set_transform(&little5, transform);

        little5.material.color = color(0.5, 1.0, 1.0);

        world_add_shape(&w, little5);
    }

    {
        cube_t frame1      = cube();
        matrix_t transform = matrix_mul(transform_translation(-10.0, 4.0, 1.0),
                                        transform_scaling(0.05, 1.0, 1.0));
        shape_set_transform(&frame1, transform);

        frame1.material.color   = color(0.7098, 0.2471, 0.2196);
        frame1.material.diffuse = 0.6;

        world_add_shape(&w, frame1);
    }

    {
        cube_t frame2      = cube();
        matrix_t transform = matrix_mul(transform_translation(-10.0, 3.4, 2.7),
                                        transform_scaling(0.05, 0.4, 0.4));
        shape_set_transform(&frame2, transform);

        frame2.material.color   = color(0.2667, 0.2706, 0.6902);
        frame2.material.diffuse = 0.6;

        world_add_shape(&w, frame2);
    }

    {
        cube_t frame3      = cube();
        matrix_t transform = matrix_mul(transform_translation(-10.0, 4.6, 2.7),
                                        transform_scaling(0.05, 0.4, 0.4));
        shape_set_transform(&frame3, transform);

        frame3.material.color   = color(0.3098, 0.5961, 0.3098);
        frame3.material.diffuse = 0.6;

        world_add_shape(&w, frame3);
    }

    {
        cube_t mirror_frame = cube();
        matrix_t transform  = matrix_mul(transform_translation(-2.0, 3.5, 9.95),
                                         transform_scaling(5.0, 1.5, 0.05));
        shape_set_transform(&mirror_frame, transform);

        mirror_frame.material.color   = color(0.3882, 0.2627, 0.1882);
        mirror_frame.material.diffuse = 0.7;

        world_add_shape(&w, mirror_frame);
    }

    {
        cube_t mirror      = cube();
        matrix_t transform = matrix_mul(transform_translation(-2.0, 3.5, 9.95),
                                        transform_scaling(4.8, 1.4, 0.06));
        shape_set_transform(&mirror, transform);

        mirror.material.color      = color(0.0, 0.0, 0.0);
        mirror.material.diffuse    = 0.0;
        mirror.material.ambient    = 0.0;
        mirror.material.specular   = 1.0;
        mirror.material.shininess  = 300.0;
        mirror.material.reflective = 1.0;

        world_add_shape(&w, mirror);
    }

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render table scene\n");
        world_free(&w);
        return false;
    }

    if (!canvas_save(image, "../renders/scene_table.ppm"))
    {
        printf("Failed to save table scene\n");
        canvas_free(image);
        world_free(&w);
        return false;
    }

    canvas_free(image);
    world_free(&w);
    return true;
}