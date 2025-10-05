// scene_shadow_glamour.c

#include "../../include/lights.h"
#include "../../include/scenes.h"
#include "../../include/sequences.h"

bool scene_shadow_glamour(void)
{

    world_t w = world();

    plane_t floor           = plane();
    floor.material.color    = color(1, 1, 1);
    floor.material.ambient  = 0.025;
    floor.material.diffuse  = 0.67;
    floor.material.specular = 0.0;
    world_add_shape(&w, floor);

    sphere_t sphere1            = sphere();
    sphere1.material.color      = color(1, 0, 0);
    sphere1.material.ambient    = 0.1;
    sphere1.material.diffuse    = 0.6;
    sphere1.material.specular   = 0.0;
    sphere1.material.reflective = 0.3;

    matrix_t sphere1_transform = matrix_mul(transform_translation(0.5, 0.5, 0),
                                            transform_scaling(0.5, 0.5, 0.5));
    shape_set_transform(&sphere1, sphere1_transform);
    world_add_shape(&w, sphere1);

    sphere_t sphere2            = sphere();
    sphere2.material.color      = color(0.5, 0.5, 1);
    sphere2.material.ambient    = 0.1;
    sphere2.material.diffuse    = 0.6;
    sphere2.material.specular   = 0.0;
    sphere2.material.reflective = 0.3;

    matrix_t sphere2_transform =
        matrix_mul(transform_translation(-0.25, 0.33, 0),
                   transform_scaling(0.33, 0.33, 0.33));
    shape_set_transform(&sphere2, sphere2_transform);
    world_add_shape(&w, sphere2);

    cube_t light_cube                = cube();
    light_cube.material.color        = color(1.5, 1.5, 1.5);
    light_cube.material.ambient      = 1.0;
    light_cube.material.diffuse      = 0.0;
    light_cube.material.specular     = 0.0;
    light_cube.material.casts_shadow = false;

    matrix_t cube_transform = matrix_mul(transform_translation(0, 3, 4),
                                         transform_scaling(1, 1, 0.01));
    shape_set_transform(&light_cube, cube_transform);
    world_add_shape(&w, light_cube);

    light_t area_light = lights_area_light(point(-1, 2, 4), vector(2, 0, 0), 10,
                                           vector(0, 2, 0), 10, color(1.5, 1.5, 1.5));

    double jitter_values[] = {0.1,  0.7,  0.3,  0.9,  0.5,  0.2,  0.8,
                              0.4,  0.6,  0.15, 0.85, 0.35, 0.65, 0.25,
                              0.75, 0.45, 0.55, 0.95, 0.05, 0.12};
    area_light.jitter_by   = sequence_from_array(jitter_values, 20);
    world_add_light(&w, area_light);

    camera_t c = camera(400 * 8, 160 * 8, 0.7854);
    matrix_t view_transform =
        transform_view(point(-3, 1, 2.5), point(0, 0.5, 0), vector(0, 1, 0));
    camera_set_transform(&c, view_transform);

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render shadow glamour scene\n");
        world_free(&w);
        return false;
    }

    if (!canvas_save(image, "../renders/scene_shadow_glamour.ppm"))
    {
        printf("Failed to save shadow glamour scene\n");
        canvas_free(image);
        world_free(&w);
        return false;
    }

    canvas_free(image);
    world_free(&w);
    return true;
}