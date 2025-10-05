// scene_teapot.c

#include "../../include/bounds.h"
#include "../../include/lights.h"
#include "../../include/obj_parser.h"
#include "../../include/scenes.h"
#include <stdio.h>
#include <stdlib.h>

bool scene_teapot(void)
{
    world_t w = world();
    cube_t light_cube                = cube();
    light_cube.material.color        = color(1.5, 1.5, 1.5);
    light_cube.material.ambient      = 1.0;
    light_cube.material.diffuse      = 0.0;
    light_cube.material.specular     = 0.0;
    light_cube.material.casts_shadow = false;

    matrix_t cube_transform = matrix_mul(transform_translation(8, 8, -10),
                                         transform_scaling(2, 2, 0.01));
    shape_set_transform(&light_cube, cube_transform);
    world_add_shape(&w, light_cube);

    world_add_light(&w, lights_area_light(point(8, 8, -10), vector(2, 0, 0), 10,
                                          vector(0, 2, 0), 10,
                                          color(1.5, 1.5, 1.5)));

    world_add_light(&w, lights_area_light(point(-2, 2, 0), vector(3, 0, 0), 10,
                                          vector(0, 1, 0), 10,
                                          color(0.7, 0.7, 0.7)));

    {
        plane_t floor                   = plane();
        floor.material.color            = hex_color("#ffffff");
        floor.material.ambient          = 0.1;
        floor.material.diffuse          = 1;
        floor.material.specular         = 0.0;
        floor.material.shininess        = 10.0;
        floor.material.reflective       = 0.0;
        floor.material.transparency     = 0.0;
        floor.material.refractive_index = 1.0;
        floor.material.casts_shadow = false;
        world_add_shape(&w, floor);
    }

    {
        cube_t backdrop_wall                    = cube();
        backdrop_wall.material.color            = hex_color("#ffffff");
        backdrop_wall.material.ambient          = 0.2;
        backdrop_wall.material.diffuse          = 0.8;
        backdrop_wall.material.specular         = 0.0;
        backdrop_wall.material.shininess        = 10.0;
        backdrop_wall.material.reflective       = 0.0;
        backdrop_wall.material.transparency     = 0.0;
        backdrop_wall.material.refractive_index = 1.0;
        backdrop_wall.material.casts_shadow     = false;
        matrix_t wall_transform = matrix_mul(transform_translation(0, 15, 3),
                                             transform_scaling(20, 15, 0.01));
        shape_set_transform((shape_t *)&backdrop_wall, wall_transform);
        world_add_shape(&w, backdrop_wall);
    }

    {
        FILE *teapot_file = fopen("../src/scenes/obj/teapot.obj", "r");
        if (!teapot_file)
        {
            printf("Failed to open teapot.obj file\n");
            world_free(&w);
            return false;
        }

        obj_parser_t parser = obj_parse_file(teapot_file);
        fclose(teapot_file);

        group_t *teapot_group = obj_parser_get_default_group(&parser);
        if (teapot_group == NULL || teapot_group->child_count == 0)
        {
            printf("No triangles found in teapot.obj\n");
            obj_parser_free(&parser);
            world_free(&w);
            return false;
        }

        material_t glass       = material();
        glass.color            = hex_color("#660033");
        glass.ambient          = 0.05;
        glass.diffuse          = 0.09;
        glass.specular         = 0.1;
        glass.shininess        = 300;
        glass.reflective       = 0.1;
        glass.transparency     = 0.7;
        glass.refractive_index = 1.52;

        material_t ceramic = material();
        ceramic.color      = hex_color("#B31B1B");
        ceramic.ambient    = 0.1;
        ceramic.diffuse    = 0.6;
        ceramic.specular   = 0.0;
        ceramic.reflective = 0.3;

        for (unsigned i = 0; i < teapot_group->child_count; i++)
        {
            shape_t *child = teapot_group->children[i];
            if (child->type == SHAPE_TRIANGLE)
            {
                triangle_t *triangle = (triangle_t *)child;
                triangle->material   = ceramic;
            }
            else if (child->type == SHAPE_SMOOTH_TRIANGLE)
            {
                smooth_triangle_t *smooth_triangle = (smooth_triangle_t *)child;
                smooth_triangle->material          = ceramic;
            }
        }

        matrix_t teapot_transform =
            matrix_mul(matrix_mul(transform_translation(0, 0, 0),
                                  transform_scaling(0.5, 0.5, 0.5)),
                       transform_rotation_y(1));
        shape_set_transform((shape_t *)teapot_group, teapot_transform);

        divide((shape_t *)teapot_group, 1);

        world_add_group(&w, teapot_group);
        parser.default_group = NULL;
        obj_parser_free(&parser);
    }

    camera_t c = camera(1200, 1000, 0.6);

    tuple_t from = point(0, 2, -8);
    tuple_t to   = point(0, 1, 0);
    tuple_t up   = vector(0, 1, 0);

    matrix_t view_transform = transform_view(from, to, up);
    camera_set_transform(&c, view_transform);

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render teapot scene\n");
        world_free(&w);
        return false;
    }

    if (!canvas_save(image, "../renders/scene_teapot.ppm"))
    {
        printf("Failed to save teapot scene\n");
        canvas_free(image);
        world_free(&w);
        return false;
    }

    canvas_free(image);
    world_free(&w);
    return true;
}