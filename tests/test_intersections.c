// test_intersections.c

#include "../include/intersections.h"
#include "../include/shapes.h"
#include "../include/tuples.h"
#include <assert.h>
#include <stdio.h>

void test_intersections(void)
{
    { // An intersection encapsulates t and object
        sphere_t s       = sphere();
        intersection_t i = intersection(3.5, &s);

        assert(equal(i.t, 3.5));
        assert(i.object == &s);
    }

    { // Intersecting a ray with a sphere
        ray_t r    = ray(point(0, 0, -5), vector(0, 0, 1));
        sphere_t s = sphere();

        intersections_t xs = sphere_intersect(&s, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
        assert(xs.intersections[0].object == &s);
        assert(xs.intersections[1].object == &s);
    }

    { // Aggregating intersections
        sphere_t s        = sphere();
        intersection_t i1 = intersection(1.0, &s);
        intersection_t i2 = intersection(2.0, &s);

        intersections_t xs = intersections(2, i1, i2);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 1.0));
        assert(equal(xs.intersections[1].t, 2.0));
    }

    { // The hit, when all intersections have positive t
        sphere_t s         = sphere();
        intersection_t i1  = intersection(1.0, &s);
        intersection_t i2  = intersection(2.0, &s);
        intersections_t xs = intersections(2, i2, i1);

        intersection_t *i = intersections_hit(&xs);

        assert(equal(i->t, xs.intersections[1].t));
        assert(i->object == xs.intersections[1].object);
    }

    { // The hit, when some intersections have negative t
        sphere_t s         = sphere();
        intersection_t i1  = intersection(-1.0, &s);
        intersection_t i2  = intersection(1.0, &s);
        intersections_t xs = intersections(2, i2, i1);

        intersection_t *i = intersections_hit(&xs);

        assert(equal(i->t, xs.intersections[0].t));
        assert(i->object == xs.intersections[0].object);
    }

    { // The hit, when all intersections have negative t
        sphere_t s         = sphere();
        intersection_t i1  = intersection(-2.0, &s);
        intersection_t i2  = intersection(-1.0, &s);
        intersections_t xs = intersections(2, i2, i1);

        intersection_t *i = intersections_hit(&xs);

        assert(i == NULL);
    }

    { // The hit is always the lowest non-negative intersection
        sphere_t s         = sphere();
        intersection_t i1  = intersection(5.0, &s);
        intersection_t i2  = intersection(7.0, &s);
        intersection_t i3  = intersection(-3.0, &s);
        intersection_t i4  = intersection(2.0, &s);
        intersections_t xs = intersections(4, i1, i2, i3, i4);

        intersection_t *i = intersections_hit(&xs);

        assert(equal(i->t, xs.intersections[3].t));
        assert(i->object == xs.intersections[3].object);
    }

    { // Precomputing the state of an intersection
        ray_t r              = ray(point(0, 0, -5), vector(0, 0, 1));
        sphere_t s           = sphere();
        intersection_t i     = intersection(4.0, &s);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);

        assert(equal(comps.t, i.t));
        assert(comps.object == i.object);
        assert(tuple_equal(comps.point, point(0, 0, -1)));
        assert(tuple_equal(comps.eyev, vector(0, 0, -1)));
        assert(tuple_equal(comps.normalv, vector(0, 0, -1)));
    }

    { // The hit, when an intersection occurs on the outside
        ray_t r              = ray(point(0, 0, -5), vector(0, 0, 1));
        sphere_t s           = sphere();
        intersection_t i     = intersection(4.0, &s);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);

        assert(comps.inside == false);
    }

    { // The hit, when an intersection occurs from the inside
        ray_t r              = ray(point(0, 0, 0), vector(0, 0, 1));
        sphere_t s           = sphere();
        intersection_t i     = intersection(1.0, &s);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);

        assert(tuple_equal(comps.point, point(0, 0, 1)));
        assert(tuple_equal(comps.eyev, vector(0, 0, -1)));
        assert(comps.inside == true);
        assert(tuple_equal(comps.normalv, vector(0, 0, -1)));
    }

    { // The hit should offset the point
        ray_t r    = ray(point(0, 0, -5), vector(0, 0, 1));
        sphere_t s = sphere();
        sphere_set_transform(&s, transform_translation(0, 0, 1));
        intersection_t i     = intersection(5.0, &s);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);

        assert(comps.over_point.z < -EPSILON / 2.0);
        assert(comps.point.z > comps.over_point.z);
    }

    { // Precomputing the reflection vector

        plane_t p = plane();
        ray_t r   = ray(point(0, 1, -1), vector(0, -sqrt(2) / 2, sqrt(2) / 2));
        intersection_t i     = intersection(sqrt(2) / 2, &p);
        computations_t comps = intersections_prepare_computations(&i, &r, NULL);
        assert(
            tuple_equal(comps.reflectv, vector(0, sqrt(2) / 2, sqrt(2) / 2)));
    }

    { // Finding n1 and n2 at various intersections

        sphere_t A = glass_sphere();
        sphere_set_transform(&A, transform_scaling(2, 2, 2));
        A.material.refractive_index = 1.5;

        sphere_t B = glass_sphere();
        sphere_set_transform(&B, transform_translation(0, 0, -0.25));
        B.material.refractive_index = 2.0;

        sphere_t C = glass_sphere();
        sphere_set_transform(&C, transform_translation(0, 0, 0.25));
        C.material.refractive_index = 2.5;

        ray_t r = ray(point(0, 0, -4), vector(0, 0, 1));

        intersection_t i1 = intersection(2.0, &A);
        intersection_t i2 = intersection(2.75, &B);
        intersection_t i3 = intersection(3.25, &C);
        intersection_t i4 = intersection(4.75, &B);
        intersection_t i5 = intersection(5.25, &C);
        intersection_t i6 = intersection(6.0, &A);

        intersections_t xs = intersections(6, i1, i2, i3, i4, i5, i6);

        computations_t comps;

        comps =
            intersections_prepare_computations(&xs.intersections[0], &r, &xs);
        assert(equal(comps.n1, 1.0));
        assert(equal(comps.n2, 1.5));

        comps =
            intersections_prepare_computations(&xs.intersections[1], &r, &xs);
        assert(equal(comps.n1, 1.5));
        assert(equal(comps.n2, 2.0));

        comps =
            intersections_prepare_computations(&xs.intersections[2], &r, &xs);
        assert(equal(comps.n1, 2.0));
        assert(equal(comps.n2, 2.5));

        comps =
            intersections_prepare_computations(&xs.intersections[3], &r, &xs);
        assert(equal(comps.n1, 2.5));
        assert(equal(comps.n2, 2.5));

        comps =
            intersections_prepare_computations(&xs.intersections[4], &r, &xs);
        assert(equal(comps.n1, 2.5));
        assert(equal(comps.n2, 1.5));

        comps =
            intersections_prepare_computations(&xs.intersections[5], &r, &xs);
        assert(equal(comps.n1, 1.5));
        assert(equal(comps.n2, 1.0));
    }

    { // The under point is offset below the surface
        ray_t r        = ray(point(0, 0, -5), vector(0, 0, 1));
        sphere_t shape = glass_sphere();
        sphere_set_transform(&shape, transform_translation(0, 0, 1));
        intersection_t i     = intersection(5, &shape);
        intersections_t xs   = intersections(1, i);
        computations_t comps = intersections_prepare_computations(&i, &r, &xs);
        assert(comps.under_point.z > EPSILON / 2);
        assert(comps.point.z < comps.under_point.z);
    }

    { // The Schlick approximation under total internal reflection
        sphere_t s         = glass_sphere();
        ray_t r            = ray(point(0, 0, sqrt(2) / 2), vector(0, 1, 0));
        intersection_t i1  = intersection(-sqrt(2) / 2, &s);
        intersection_t i2  = intersection(sqrt(2) / 2, &s);
        intersections_t xs = intersections(2, i1, i2);
        computations_t comps =
            intersections_prepare_computations(&xs.intersections[1], &r, &xs);
        double reflectance = intersections_shlick(&comps);
        assert(equal(reflectance, 1.0));
    }

    { // The Schlick approximation with a perpendicular viewing angle
        sphere_t s         = glass_sphere();
        ray_t r            = ray(point(0, 0, 0), vector(0, 1, 0));
        intersection_t i1  = intersection(-1, &s);
        intersection_t i2  = intersection(1, &s);
        intersections_t xs = intersections(2, i1, i2);
        computations_t comps =
            intersections_prepare_computations(&xs.intersections[1], &r, &xs);
        double reflectance = intersections_shlick(&comps);
        assert(equal(reflectance, 0.04));
    }

    { // An intersection can encapsulate u and v
        triangle_t s =
            triangle(point(0, 1, 0), point(-1, 0, 0), point(1, 0, 0));
        intersection_t i = intersection_with_uv(3.5, &s, 0.2, 0.4);

        assert(equal(i.u, 0.2));
        assert(equal(i.v, 0.4));
    }
}

int main(void)
{
    test_intersections();
    return 0;
}
