// test_materials.c

#include "../include/materials.h"
#include "../include/shapes.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

void test_materials(void)
{
    { // Testing the default material
        material_t m = material();

        assert(tuple_equal(m.color, color(1, 1, 1)));
        assert(equal(m.ambient, 0.1));
        assert(equal(m.diffuse, 0.9));
        assert(equal(m.specular, 0.9));
        assert(equal(m.shininess, 200.0));
    }

    { // Lighting with the eye between light and surface
        sphere_t s       = sphere();
        material_t m     = material();
        tuple_t position = point(0, 0, 0);
        tuple_t eyev     = vector(0, 0, -1);
        tuple_t normalv  = vector(0, 0, -1);
        light_t light    = lights_point_light(point(0, 0, -10), color(1, 1, 1));

        tuple_t result =
            materials_lighting(&m, &s, &light, position, eyev, normalv, 1.0);
        assert(tuple_equal(result, color(1.9, 1.9, 1.9)));
    }

    { // Lighting with eye offset 45 degrees
        sphere_t s       = sphere();
        material_t m     = material();
        tuple_t position = point(0, 0, 0);
        tuple_t eyev     = vector(0, sqrt(2.0) / 2.0, -sqrt(2.0) / 2.0);
        tuple_t normalv  = vector(0, 0, -1);
        light_t light    = lights_point_light(point(0, 0, -10), color(1, 1, 1));

        tuple_t result =
            materials_lighting(&m, &s, &light, position, eyev, normalv, 1.0);
        assert(tuple_equal(result, color(1.0, 1.0, 1.0)));
    }

    { // Lighting with light offset 45 degrees
        sphere_t s       = sphere();
        material_t m     = material();
        tuple_t position = point(0, 0, 0);
        tuple_t eyev     = vector(0, 0, -1);
        tuple_t normalv  = vector(0, 0, -1);
        light_t light = lights_point_light(point(0, 10, -10), color(1, 1, 1));

        tuple_t result =
            materials_lighting(&m, &s, &light, position, eyev, normalv, 1.0);
        assert(tuple_equal(result, color(0.7364, 0.7364, 0.7364)));
    }

    { // Lighting with eye in path of reflection vector
        sphere_t s       = sphere();
        material_t m     = material();
        tuple_t position = point(0, 0, 0);
        tuple_t eyev     = vector(0, -sqrt(2.0) / 2.0, -sqrt(2.0) / 2.0);
        tuple_t normalv  = vector(0, 0, -1);
        light_t light = lights_point_light(point(0, 10, -10), color(1, 1, 1));

        tuple_t result =
            materials_lighting(&m, &s, &light, position, eyev, normalv, 1.0);
        assert(tuple_equal(result, color(1.6364, 1.6364, 1.6364)));
    }

    { // Lighting with light behind surface
        sphere_t s       = sphere();
        material_t m     = material();
        tuple_t position = point(0, 0, 0);
        tuple_t eyev     = vector(0, 0, -1);
        tuple_t normalv  = vector(0, 0, -1);
        light_t light    = lights_point_light(point(0, 0, 10), color(1, 1, 1));

        tuple_t result =
            materials_lighting(&m, &s, &light, position, eyev, normalv, 1.0);
        assert(tuple_equal(result, color(0.1, 0.1, 0.1)));
    }

    { // Lighting with surface in shadow
        sphere_t s       = sphere();
        material_t m     = material();
        tuple_t position = point(0, 0, 0);
        tuple_t eyev     = vector(0, 0, -1);
        tuple_t normalv  = vector(0, 0, -1);
        light_t light    = lights_point_light(point(0, 0, -10), color(1, 1, 1));

        tuple_t result =
            materials_lighting(&m, &s, &light, position, eyev, normalv, 0.0);
        assert(tuple_equal(result, color(0.1, 0.1, 0.1)));
    }

    { // Lighting with a pattern
        sphere_t s    = sphere();
        material_t m  = material();
        m.has_pattern = true;
        m.pattern     = patterns_stripe(WHITE, BLACK);
        m.ambient     = 1.0;
        m.diffuse     = 0.0;
        m.specular    = 0.0;

        tuple_t eyev    = vector(0, 0, -1);
        tuple_t normalv = vector(0, 0, -1);
        light_t light   = lights_point_light(point(0, 0, -10), color(1, 1, 1));

        tuple_t c1 = materials_lighting(&m, &s, &light, point(0.9, 0, 0), eyev,
                                        normalv, 1.0);
        tuple_t c2 = materials_lighting(&m, &s, &light, point(1.1, 0, 0), eyev,
                                        normalv, 1.0);

        assert(tuple_equal(c1, WHITE));
        assert(tuple_equal(c2, BLACK));
    }

    { // Reflectivity for the default material

        material_t m = material();
        assert(equal(m.reflective, 0.0));
    }

    { // Transparency and Refractive Index for the default material

        material_t m = material();
        assert(equal(m.transparency, 0.0));
        assert(equal(m.refractive_index, 1.0));
    }

    { // lighting() uses light intensity to attenuate color
        sphere_t shape          = sphere();
        shape.material.ambient  = 0.1;
        shape.material.diffuse  = 0.9;
        shape.material.specular = 0.0;
        shape.material.color    = color(1, 1, 1);

        tuple_t pt      = point(0, 0, -1);
        tuple_t eyev    = vector(0, 0, -1);
        tuple_t normalv = vector(0, 0, -1);
        light_t light   = lights_point_light(point(0, 0, -10), color(1, 1, 1));

        tuple_t result1 = materials_lighting(&shape.material, &shape, &light,
                                             pt, eyev, normalv, 1.0);
        assert(tuple_equal(result1, color(1, 1, 1)));

        tuple_t result2 = materials_lighting(&shape.material, &shape, &light,
                                             pt, eyev, normalv, 0.5);
        assert(tuple_equal(result2, color(0.55, 0.55, 0.55)));

        tuple_t result3 = materials_lighting(&shape.material, &shape, &light,
                                             pt, eyev, normalv, 0.0);
        assert(tuple_equal(result3, color(0.1, 0.1, 0.1)));
    }

    { // lighting() samples the area light - case 1
        tuple_t corner = point(-0.5, -0.5, -5);
        tuple_t v1     = vector(1, 0, 0);
        tuple_t v2     = vector(0, 1, 0);
        light_t light = lights_area_light(corner, v1, 2, v2, 2, color(1, 1, 1));
        sphere_t shape          = sphere();
        shape.material.ambient  = 0.1;
        shape.material.diffuse  = 0.9;
        shape.material.specular = 0.0;
        shape.material.color    = color(1, 1, 1);
        tuple_t eye             = point(0, 0, -5);
        tuple_t pt              = point(0, 0, -1);
        tuple_t eyev            = tuple_normalize(tuple_subtract(eye, pt));
        tuple_t normalv         = vector(pt.x, pt.y, pt.z);

        tuple_t result = materials_lighting(&shape.material, &shape, &light, pt,
                                            eyev, normalv, 1.0);
        assert(tuple_equal(result, color(0.9965, 0.9965, 0.9965)));
    }

    { // lighting() samples the area light - case 2
        tuple_t corner = point(-0.5, -0.5, -5);
        tuple_t v1     = vector(1, 0, 0);
        tuple_t v2     = vector(0, 1, 0);
        light_t light = lights_area_light(corner, v1, 2, v2, 2, color(1, 1, 1));
        sphere_t shape          = sphere();
        shape.material.ambient  = 0.1;
        shape.material.diffuse  = 0.9;
        shape.material.specular = 0.0;
        shape.material.color    = color(1, 1, 1);
        tuple_t eye             = point(0, 0, -5);
        tuple_t pt              = point(0, 0.7071, -0.7071);
        tuple_t eyev            = tuple_normalize(tuple_subtract(eye, pt));
        tuple_t normalv         = vector(pt.x, pt.y, pt.z);

        tuple_t result = materials_lighting(&shape.material, &shape, &light, pt,
                                            eyev, normalv, 1.0);
        assert(tuple_equal(result, color(0.6232, 0.6232, 0.6232)));
    }
}

int main(void)
{
    test_materials();
    return 0;
}
