#include "../../include/bounds.h"
#include "../../include/lights.h"
#include "../../include/scenes.h"
#include "../../include/shapes.h"
#include <stdio.h>
#include <stdlib.h>

bool scene_sphere_grid(void)
{
    world_t w = world();

    world_add_light(&w, lights_point_light(point(0, 50, 0), color(1, 1, 1)));

    material_t sphere_material       = material();
    sphere_material.diffuse          = 1;
    sphere_material.specular         = 0;
    sphere_material.shininess        = 0;
    sphere_material.reflective       = 0;
    sphere_material.transparency     = 0;
    sphere_material.refractive_index = 1;

    {
        group_t *grid = group();

        for (int x = 0; x < 10; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                for (int z = 0; z < 10; z++)
                {
                    sphere_t *s       = malloc(sizeof(sphere_t));
                    *s                = sphere();
                    s->material       = sphere_material;
                    s->material.color = hex_color("#ed80e9");

                    matrix_t translate =
                        transform_translation(x * 2.5, y * 2.5, z * 2.5);
                    matrix_t scale     = transform_scaling(0.5, 0.5, 0.5);
                    matrix_t transform = matrix_mul(translate, scale);
                    shape_set_transform(s, transform);

                    group_add_child(grid, (shape_t *)s);
                }
            }
        }

        divide((shape_t *)grid, 1);

        world_add_group(&w, grid);
    }

    camera_t c = camera(2000, 2000, M_PI_3);

    tuple_t from = point(11.25, 11.25, -30);
    tuple_t to   = point(11.25, 11.25, 11.25);
    tuple_t up   = vector(0, 1, 0);

    matrix_t view_transform = transform_view(from, to, up);
    camera_set_transform(&c, view_transform);

    canvas_t *image = camera_render(&c, &w);
    if (!image)
    {
        printf("Failed to render sphere grid scene\n");
        world_free(&w);
        return false;
    }

    if (!canvas_save(image, "../renders/scene_sphere_grid.ppm"))
    {
        printf("Failed to save sphere grid scene\n");
        canvas_free(image);
        world_free(&w);
        return false;
    }

    printf("Sphere grid scene rendered successfully\n");
    canvas_free(image);
    world_free(&w);
    return true;
}
