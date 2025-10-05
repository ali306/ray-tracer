// scene_dragon.c

#include "../../include/bounds.h"
#include "../../include/lights.h"
#include "../../include/obj_parser.h"
#include "../../include/scenes.h"
#include <stdio.h>
#include <stdlib.h>

bool scene_dragon(void)
{
    world_t w = world();

    world_add_light(&w,
                    lights_point_light(point(-30, 30, -30), color(3.5, 3.5, 3.5)));

    /* world_add_light(&w, lights_area_light(point(-30, 30, -30), vector(2, 0, 0), 2,
                                    vector(0, 2, 0), 2, color(3.8, 3.8, 3.8))); */

    {
        plane_t floor             = plane();
        floor.material            = material();
        floor.material.color      = hex_color("#000000");
        floor.material.ambient    = 1;
        floor.material.diffuse    = 0.6;
        floor.material.specular   = 0.4;
        floor.material.shininess  = 50.0;
        floor.material.reflective = 0.3;
        world_add_shape(&w, floor);
    }

    {
        FILE *dragon_file = fopen("../src/scenes/obj/dragon.obj", "r");
        if (!dragon_file)
        {
            printf("Failed to open dragon.obj file\n");
            world_free(&w);
            return false;
        }

        obj_parser_t parser = obj_parse_file(dragon_file);
        fclose(dragon_file);

        group_t *dragon_group = obj_parser_get_default_group(&parser);
        if (dragon_group == NULL || dragon_group->child_count == 0)
        {
            printf("No triangles found in dragon.obj\n");
            obj_parser_free(&parser);
            world_free(&w);
            return false;
        }

        material_t jade_material       = material();
        jade_material.color            = hex_color("#2e6f40");
        jade_material.ambient          = 0.1;
        jade_material.diffuse          = 0.25;
        jade_material.specular         = 0.9;
        jade_material.shininess        = 300.0;
        jade_material.reflective       = 0.15;
        jade_material.transparency     = 1.6;
        jade_material.refractive_index = 5;

        for (unsigned i = 0; i < dragon_group->child_count; i++)
        {
            shape_t *child = dragon_group->children[i];
            if (child->type == SHAPE_TRIANGLE)
            {
                triangle_t *triangle = (triangle_t *)child;
                triangle->material   = jade_material;
            }
            else if (child->type == SHAPE_SMOOTH_TRIANGLE)
            {
                smooth_triangle_t *smooth_triangle = (smooth_triangle_t *)child;
                smooth_triangle->material          = jade_material;
            }
        }

        matrix_t dragon_transform =
            matrix_mul(matrix_mul(transform_translation(0, 0, 0),
                                  transform_scaling(1.0, 1.0, 1.0)),
                       transform_rotation_y(M_PI_2));
        shape_set_transform((shape_t *)dragon_group, dragon_transform);

        divide((shape_t *)dragon_group, 1);

        world_add_group(&w, dragon_group);
        parser.default_group = NULL;
        obj_parser_free(&parser);
    }

    camera_t c = camera(1000, 1000, 0.2);

    tuple_t from = point(-50, 2, 45);
    tuple_t to   = point(1, 2, 0);
    tuple_t up   = vector(0, 1, 0);

    matrix_t view_transform = transform_view(from, to, up);
    camera_set_transform(&c, view_transform);

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render dragon scene\n");
        world_free(&w);
        return false;
    }

    if (!canvas_save(image, "../renders/scene_dragon.ppm"))
    {
        printf("Failed to save dragon scene\n");
        canvas_free(image);
        world_free(&w);
        return false;
    }

    canvas_free(image);
    world_free(&w);
    return true;
}
