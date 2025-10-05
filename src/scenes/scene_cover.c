// scenes_cover.c

#include "../../include/lights.h"
#include "../../include/scenes.h"
#include "../../include/shapes.h"
#include <stdlib.h>

typedef struct
{
    material_t *material;
    matrix_t *transform;
    tuple_t position;
} cube_parameters_t;

bool scene_cover(void)
{

    world_t w = world();

    world_add_light(&w, lights_point_light(point(50, 100, -50), color(1, 1, 1)));
    world_add_light(&w, lights_point_light(point(-400, 50, -10), color(0.2, 0.2, 0.2)));

    material_t white_material = material();
    white_material.color      = color(1.0, 1.0, 1.0);
    white_material.diffuse    = 0.7;
    white_material.ambient    = 0.1;
    white_material.specular   = 0;
    white_material.reflective = 0.1;

    material_t blue_material = white_material;
    blue_material.color      = color(0.537, 0.831, 0.914);

    material_t red_material    = white_material;
    red_material.color         = color(0.941, 0.322, 0.388);
    material_t purple_material = white_material;
    purple_material.color      = color(0.373, 0.404, 0.550);

    matrix_t translate_standard = transform_translation(1.0, -1.0, 1.0);
    matrix_t scale_standard     = transform_scaling(0.5, 0.5, 0.5);
    matrix_t standard_transform =
        matrix_mul(scale_standard, translate_standard);

    matrix_t scale_large  = transform_scaling(3.5, 3.5, 3.5);
    matrix_t large_object = matrix_mul(scale_large, standard_transform);

    matrix_t scale_medium  = transform_scaling(3.0, 3.0, 3.0);
    matrix_t medium_object = matrix_mul(scale_medium, standard_transform);

    matrix_t scale_small  = transform_scaling(2.0, 2.0, 2.0);
    matrix_t small_object = matrix_mul(scale_small, standard_transform);

    {
        plane_t p           = plane();
        p.material          = material();
        p.material.color    = color(1.0, 1.0, 1.0);
        p.material.ambient  = 1.0;
        p.material.diffuse  = 0.0;
        p.material.specular = 0.0;

        matrix_t rotate_p    = transform_rotation_x(M_PI_2);
        matrix_t translate_p = transform_translation(0.0, 0.0, 500.0);
        matrix_t p_transform = matrix_mul(translate_p, rotate_p);
        shape_set_transform(&p, p_transform);

        world_add_shape(&w, p);
    }

    {
        sphere_t s                  = glass_sphere();
        s.material.color            = color(0.373, 0.404, 0.550);
        s.material.diffuse          = 0.2;
        s.material.ambient          = 0.0;
        s.material.specular         = 1.0;
        s.material.shininess        = 200.0;
        s.material.reflective       = 0.7;
        s.material.transparency     = 0.7;
        s.material.refractive_index = 1.5;

        shape_set_transform(&s, large_object);
        world_add_shape(&w, s);
    }

    cube_parameters_t cubes[] = {
        {&white_material, &medium_object, vector(4.0, 0.0, 0.0)},
        {&blue_material, &large_object, vector(8.5, 1.5, -0.5)},
        {&red_material, &large_object, vector(0.0, 0.0, 4.0)},
        {&white_material, &small_object, vector(4.0, 0.0, 4.0)},
        {&purple_material, &medium_object, vector(7.5, 0.5, 4.0)},
        {&white_material, &medium_object, vector(-0.25, 0.25, 8.0)},
        {&blue_material, &large_object, vector(4.0, 1.0, 7.5)},
        {&red_material, &medium_object, vector(10.0, 2.0, 7.5)},
        {&white_material, &small_object, vector(8.0, 2.0, 12.0)},
        {&white_material, &small_object, vector(20.0, 1.0, 9.0)},
        {&blue_material, &large_object, vector(-0.5, -5.0, 0.25)},
        {&red_material, &large_object, vector(4.0, -4.0, 0.0)},
        {&white_material, &large_object, vector(8.5, -4.0, 0.0)},
        {&white_material, &large_object, vector(0.0, -4.0, 4.0)},
        {&purple_material, &large_object, vector(-0.5, -4.5, 8.0)},
        {&white_material, &large_object, vector(0.0, -8.0, 4.0)},
        {&white_material, &large_object, vector(-0.5, -8.5, 8.0)}};

    int num_cubes = sizeof(cubes) / sizeof(cube_parameters_t);

    for (int i = 0; i < num_cubes; i++)
    {
        cube_t new_cube   = cube();
        new_cube.material = *cubes[i].material;

        matrix_t translate_cube = transform_translation(
            cubes[i].position.x, cubes[i].position.y, cubes[i].position.z);

        matrix_t cube_transform =
            matrix_mul(translate_cube, *cubes[i].transform);
        shape_set_transform(&new_cube, cube_transform);

        world_add_shape(&w, new_cube);
    }

    camera_t c = camera(1000, 1000, 0.785);

    matrix_t view_transform =
        transform_view(point(-6, 6, -10), point(6, 0, 6), vector(-0.45, 1, 0));
    camera_set_transform(&c, view_transform);

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render cover scene\n");
        world_free(&w);
        return false;
    }
    else
    {
        if (!canvas_save(image, "../renders/scene_cover.ppm"))
        {
            printf("Failed to save cover scene\n");
        }
        canvas_free(image);
    }

    world_free(&w);
    return true;
}
