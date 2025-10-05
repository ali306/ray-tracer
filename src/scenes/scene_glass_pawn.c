#include "../../include/lights.h"
#include "../../include/obj_parser.h"
#include "../../include/patterns.h"
#include "../../include/scenes.h"
#include "../../include/sequences.h"

bool scene_glass_pawn(void)
{
    world_t w = world();

    plane_t floor              = plane();
    floor.material             = material();
    floor.material.has_pattern = true;
    floor.material.pattern =
        patterns_checker(hex_color("#ffffff"), hex_color("#000000"));
    patterns_set_transform(&floor.material.pattern, transform_scaling(2, 2, 2));
    floor.material.specular   = 0.3;
    floor.material.reflective = 0.2;
    world_add_shape(&w, floor);

    FILE *pawn_file = fopen("../src/scenes/obj/pawn.obj", "r");
    if (!pawn_file)
    {
        printf("Failed to open pawn.obj file\n");
        world_free(&w);
        return false;
    }

    obj_parser_t parser = obj_parse_file(pawn_file);
    fclose(pawn_file);

    group_t *pawn = obj_parser_get_default_group(&parser);
    if (pawn == NULL || pawn->child_count == 0)
    {
        printf("No triangles found in pawn.obj\n");
        obj_parser_free(&parser);
        world_free(&w);
        return false;
    }

    material_t glass       = material();
    glass.color            = hex_color("#ffffff");
    glass.ambient          = 0.05;
    glass.diffuse          = 0.25;
    glass.specular         = 1;
    glass.shininess        = 10;
    glass.reflective       = 1;
    glass.transparency     = 0.8;
    glass.refractive_index = 2 * 1.52;
    glass.casts_shadow     = false;

    for (unsigned i = 0; i < pawn->child_count; i++)
    {
        shape_t *child = pawn->children[i];
        if (child->type == SHAPE_TRIANGLE)
        {
            triangle_t *triangle = (triangle_t *)child;
            triangle->material   = glass;
        }
        else if (child->type == SHAPE_SMOOTH_TRIANGLE)
        {
            smooth_triangle_t *smooth_triangle = (smooth_triangle_t *)child;
            smooth_triangle->material          = glass;
        }
    }

    matrix_t pawn_transform =
        matrix_mul(transform_translation(12.12, -.5, -10.15),
                   transform_scaling(0.5, 0.5, 0.5));
    shape_set_transform((shape_t *)pawn, pawn_transform);

    divide((shape_t *)pawn, 1);

    world_add_group(&w, pawn);
    parser.default_group = NULL;
    obj_parser_free(&parser);

    light_t area_light = lights_area_light(point(10, 13, 0), vector(1, 0, 0), 1,
                                           vector(0, 1, 0), 1, color(1, 1, 1));

    double jitter_values[] = {0.1,  0.7,  0.3,  0.9,  0.5,  0.2,  0.8,
                              0.4,  0.6,  0.15, 0.85, 0.35, 0.65, 0.25,
                              0.75, 0.45, 0.55, 0.95, 0.05, 0.12};
    area_light.jitter_by   = sequence_from_array(jitter_values, 20);
    world_add_light(&w, area_light);

    camera_t c = camera(1000, 1000, M_PI / 3);
    matrix_t view_transform =
        transform_view(point(1, 1.7, -7), point(1, 2.5, 14), vector(0, 1, 0));
    camera_set_transform(&c, view_transform);

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render glass pawn scene\n");
        world_free(&w);
        return false;
    }

    if (!canvas_save(image, "../renders/scene_glass_pawn.ppm"))
    {
        printf("Failed to save glass pawn scene\n");
        canvas_free(image);
        world_free(&w);
        return false;
    }

    canvas_free(image);
    world_free(&w);
    return true;
}
