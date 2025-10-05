// test_lights.c

#include "../include/lights.h"
#include "../include/sequences.h"
#include "../include/tuples.h"
#include "../include/world.h"
#include <assert.h>
#include <stdio.h>

void test_lights(void)
{
    { // Creating a point light
        tuple_t intensity = color(1, 1, 1);
        tuple_t position  = point(0, 0, 0);
        light_t light     = lights_point_light(position, intensity);

        assert(tuple_equal(light.intensity, intensity));
        assert(tuple_equal(light.position, position));
    }

    { // Point lights evaluate the light intensity at a given point
        world_t w     = world_default();
        light_t light = w.lights[0];

        tuple_t pt1       = point(0, 1.0001, 0);
        double intensity1 = lights_intensity_at(&light, pt1, &w);
        assert(equal(intensity1, 1.0));

        tuple_t pt2       = point(-1.0001, 0, 0);
        double intensity2 = lights_intensity_at(&light, pt2, &w);
        assert(equal(intensity2, 1.0));

        tuple_t pt3       = point(0, 0, -1.0001);
        double intensity3 = lights_intensity_at(&light, pt3, &w);
        assert(equal(intensity3, 1.0));

        tuple_t pt4       = point(0, 0, 1.0001);
        double intensity4 = lights_intensity_at(&light, pt4, &w);
        assert(equal(intensity4, 0.0));

        tuple_t pt5       = point(1.0001, 0, 0);
        double intensity5 = lights_intensity_at(&light, pt5, &w);
        assert(equal(intensity5, 0.0));

        tuple_t pt6       = point(0, -1.0001, 0);
        double intensity6 = lights_intensity_at(&light, pt6, &w);
        assert(equal(intensity6, 0.0));

        tuple_t pt7       = point(0, 0, 0);
        double intensity7 = lights_intensity_at(&light, pt7, &w);
        assert(equal(intensity7, 0.0));
        world_free(&w);
    }

    { // Creating an area light
        tuple_t corner = point(0, 0, 0);
        tuple_t v1     = vector(2, 0, 0);
        tuple_t v2     = vector(0, 0, 1);
        light_t light = lights_area_light(corner, v1, 4, v2, 2, color(1, 1, 1));

        assert(tuple_equal(light.corner, corner));
        assert(tuple_equal(light.uvec, vector(0.5, 0, 0)));
        assert(light.usteps == 4);
        assert(tuple_equal(light.vvec, vector(0, 0, 0.5)));
        assert(light.vsteps == 2);
        assert(light.samples == 8);
        assert(tuple_equal(light.position, point(1, 0, 0.5)));
    }

    { // Finding a single point on an area light
        tuple_t corner = point(0, 0, 0);
        tuple_t v1     = vector(2, 0, 0);
        tuple_t v2     = vector(0, 0, 1);
        light_t light = lights_area_light(corner, v1, 4, v2, 2, color(1, 1, 1));

        tuple_t pt1 = lights_point_on_light(&light, 0, 0);
        assert(tuple_equal(pt1, point(0.25, 0, 0.25)));

        tuple_t pt2 = lights_point_on_light(&light, 1, 0);
        assert(tuple_equal(pt2, point(0.75, 0, 0.25)));

        tuple_t pt3 = lights_point_on_light(&light, 0, 1);
        assert(tuple_equal(pt3, point(0.25, 0, 0.75)));

        tuple_t pt4 = lights_point_on_light(&light, 2, 0);
        assert(tuple_equal(pt4, point(1.25, 0, 0.25)));

        tuple_t pt5 = lights_point_on_light(&light, 3, 1);
        assert(tuple_equal(pt5, point(1.75, 0, 0.75)));
    }

    { // The area light intensity function
        world_t w      = world_default();
        tuple_t corner = point(-0.5, -0.5, -5);
        tuple_t v1     = vector(1, 0, 0);
        tuple_t v2     = vector(0, 1, 0);
        light_t light = lights_area_light(corner, v1, 2, v2, 2, color(1, 1, 1));

        tuple_t pt1       = point(0, 0, 2);
        double intensity1 = lights_intensity_at(&light, pt1, &w);
        assert(equal(intensity1, 0.0));

        tuple_t pt2       = point(1, -1, 2);
        double intensity2 = lights_intensity_at(&light, pt2, &w);
        assert(equal(intensity2, 0.25));

        tuple_t pt3       = point(1.5, 0, 2);
        double intensity3 = lights_intensity_at(&light, pt3, &w);
        assert(equal(intensity3, 0.5));

        tuple_t pt4       = point(1.25, 1.25, 3);
        double intensity4 = lights_intensity_at(&light, pt4, &w);
        assert(equal(intensity4, 0.75));

        tuple_t pt5       = point(0, 0, -2);
        double intensity5 = lights_intensity_at(&light, pt5, &w);
        assert(equal(intensity5, 1.0));
        world_free(&w);
    }

    { // Finding a single point on a jittered area light
        tuple_t corner = point(0, 0, 0);
        tuple_t v1     = vector(2, 0, 0);
        tuple_t v2     = vector(0, 0, 1);
        light_t light = lights_area_light(corner, v1, 4, v2, 2, color(1, 1, 1));

        double jitter_values[] = {0.3, 0.7};
        light.jitter_by        = sequence_from_array(jitter_values, 2);

        tuple_t pt1 = lights_point_on_light(&light, 0, 0);
        assert(tuple_equal(pt1, point(0.15, 0, 0.35)));

        tuple_t pt2 = lights_point_on_light(&light, 1, 0);
        assert(tuple_equal(pt2, point(0.65, 0, 0.35)));

        tuple_t pt3 = lights_point_on_light(&light, 0, 1);
        assert(tuple_equal(pt3, point(0.15, 0, 0.85)));

        tuple_t pt4 = lights_point_on_light(&light, 2, 0);
        assert(tuple_equal(pt4, point(1.15, 0, 0.35)));

        tuple_t pt5 = lights_point_on_light(&light, 3, 1);
        assert(tuple_equal(pt5, point(1.65, 0, 0.85)));
    }

    { // The area light with jittered samples
        world_t w      = world_default();
        tuple_t corner = point(-0.5, -0.5, -5);
        tuple_t v1     = vector(1, 0, 0);
        tuple_t v2     = vector(0, 1, 0);
        light_t light = lights_area_light(corner, v1, 2, v2, 2, color(1, 1, 1));

        double jitter_values[] = {0.7, 0.3, 0.9, 0.1, 0.5};
        light.jitter_by        = sequence_from_array(jitter_values, 5);

        tuple_t pt1       = point(0, 0, 2);
        double intensity1 = lights_intensity_at(&light, pt1, &w);
        assert(equal(intensity1, 0.0));

        light.jitter_by   = sequence_from_array(jitter_values, 5);
        tuple_t pt2       = point(1, -1, 2);
        double intensity2 = lights_intensity_at(&light, pt2, &w);
        assert(equal(intensity2, 0.5));

        light.jitter_by   = sequence_from_array(jitter_values, 5);
        tuple_t pt3       = point(1.5, 0, 2);
        double intensity3 = lights_intensity_at(&light, pt3, &w);
        assert(equal(intensity3, 0.75));

        light.jitter_by   = sequence_from_array(jitter_values, 5);
        tuple_t pt4       = point(1.25, 1.25, 3);
        double intensity4 = lights_intensity_at(&light, pt4, &w);
        assert(equal(intensity4, 0.75));

        light.jitter_by   = sequence_from_array(jitter_values, 5);
        tuple_t pt5       = point(0, 0, -2);
        double intensity5 = lights_intensity_at(&light, pt5, &w);
        assert(equal(intensity5, 1.0));
        world_free(&w);
    }
}

int main(void)
{
    test_lights();
    return 0;
}
