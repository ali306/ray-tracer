

#include "../../include/camera.h"
#include "../../include/lights.h"
#include "../../include/patterns.h"
#include "../../include/scenes.h"
#include "../../include/shapes.h"
#include "../../include/transformations.h"
#include "../../include/world.h"

bool scene_checkered(void)
{
    const unsigned size = 5000;

    world_t w = world();

    world_add_light(
        &w, lights_point_light(point(-10, 10, -10), color(1.0, 1.0, 1.0)));

    {
        plane_t p              = plane();
        p.material             = material();
        p.material.has_pattern = true;
        p.material.pattern =
            patterns_checker(color(1.0, 1.0, 1.0), color(0.2, 0.4, 0.8));
        patterns_set_transform(&p.material.pattern,
                               transform_scaling(0.5, 0.5, 0.5));
        p.material.specular   = 0.8;
        p.material.reflective = 0.2;
        world_add_shape(&w, p);
    }

    material_t metallic_silver   = material();
    metallic_silver.color        = color(0.9, 0.9, 0.9);
    metallic_silver.ambient      = 0.05;
    metallic_silver.diffuse      = 0.1;
    metallic_silver.specular     = 1.0;
    metallic_silver.shininess    = 300.0;
    metallic_silver.reflective   = 0.9;
    metallic_silver.transparency = 0.0;

    sphere_t s = glass_sphere();
    shape_set_transform(&s, transform_translation(0.0, 1.0, 0.0));
    s.material = metallic_silver;
    world_add_shape(&w, s);

    {
        plane_t p          = plane();
        matrix_t transform = matrix_mul(transform_translation(0.0, 0.0, 1000.0),
                                        transform_rotation_x((M_PI / 2)));
        shape_set_transform(&p, transform);
        p.material             = material();
        p.material.has_pattern = false;
        p.material.color       = hex_color("#becefc");
        p.material.specular    = 0.0;
        p.material.reflective  = 0.1;
        world_add_shape(&w, p);
    }

    camera_t c = camera(size, size, (M_PI / 3));

    matrix_t view_transform = transform_view(
        point(0.0, 2, -2.0), point(0.0, 1.0, 0.0), vector(0.0, 1.0, 0.0));
    camera_set_transform(&c, view_transform);

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render checkered scene\n");
        world_free(&w);
        return false;
    }
    else
    {
        if (!canvas_save(image, "../renders/scene_checkered.ppm"))
        {
            printf("Failed to save checkered scene\n");
        }
        canvas_free(image);
    }

    world_free(&w);
    return true;
}
