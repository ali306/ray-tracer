// test_spheres.c

#include "../include/shapes.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

void test_spheres(void)
{
    { // A ray intersects a sphere at two points
        tuple_t origin    = point(0, 0, -5);
        tuple_t direction = vector(0, 0, 1);
        ray_t r           = ray(origin, direction);

        sphere_t s         = sphere();
        intersections_t xs = sphere_intersect(&s, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 4.0));
        assert(equal(xs.intersections[1].t, 6.0));
    }

    { // A ray intersects a sphere at a tangent
        tuple_t origin    = point(0, 1, -5);
        tuple_t direction = vector(0, 0, 1);
        ray_t r           = ray(origin, direction);

        sphere_t s         = sphere();
        intersections_t xs = sphere_intersect(&s, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 5.0));
        assert(equal(xs.intersections[1].t, 5.0));
    }

    { // A ray misses a sphere
        tuple_t origin    = point(0, 2, -5);
        tuple_t direction = vector(0, 0, 1);
        ray_t r           = ray(origin, direction);

        sphere_t s         = sphere();
        intersections_t xs = sphere_intersect(&s, r);

        assert(xs.count == 0);
    }

    { // A ray originates inside a sphere
        tuple_t origin    = point(0, 0, 0);
        tuple_t direction = vector(0, 0, 1);
        ray_t r           = ray(origin, direction);

        sphere_t s         = sphere();
        intersections_t xs = sphere_intersect(&s, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, -1.0));
        assert(equal(xs.intersections[1].t, 1.0));
    }

    { // A sphere is behind a ray
        tuple_t origin    = point(0, 0, 5);
        tuple_t direction = vector(0, 0, 1);
        ray_t r           = ray(origin, direction);

        sphere_t s         = sphere();
        intersections_t xs = sphere_intersect(&s, r);

        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, -6.0));
        assert(equal(xs.intersections[1].t, -4.0));
    }

    { // A sphere's default transformation
        sphere_t s = sphere();
        assert(matrix_equal(s.transform, IDENTITY));
    }

    { // Changing a sphere's transformation
        sphere_t s = sphere();
        matrix_t t = transform_translation(2, 3, 4);
        sphere_set_transform(&s, t);
        assert(matrix_equal(s.transform, t));
    }

    { // Intersecting a scaled sphere with a ray
        ray_t r    = ray(point(0, 0, -5), vector(0, 0, 1));
        sphere_t s = sphere();
        shape_set_transform(&s, transform_scaling(2, 2, 2));

        intersections_t xs = shape_intersect(&s, r);
        assert(xs.count == 2);
        assert(equal(xs.intersections[0].t, 3.0));
        assert(equal(xs.intersections[1].t, 7.0));
    }

    { // Intersecting a translated sphere with a ray
        ray_t r    = ray(point(0, 0, -5), vector(0, 0, 1));
        sphere_t s = sphere();
        sphere_set_transform(&s, transform_translation(5, 0, 0));

        intersections_t xs = shape_intersect(&s, r);
        assert(xs.count == 0);
    }

    { // Computing the normal on a sphere
        sphere_t s = sphere();

        tuple_t n1 = shape_normal_at(&s, point(1, 0, 0), NULL);
        assert(tuple_equal(n1, vector(1, 0, 0)));

        tuple_t n2 = shape_normal_at(&s, point(0, 1, 0), NULL);
        assert(tuple_equal(n2, vector(0, 1, 0)));

        tuple_t n3 = shape_normal_at(&s, point(0, 0, 1), NULL);
        assert(tuple_equal(n3, vector(0, 0, 1)));

        double sqrt3_div3 = sqrt(3.0) / 3.0;
        tuple_t n4        = shape_normal_at(
            &s, point(sqrt3_div3, sqrt3_div3, sqrt3_div3), NULL);
        assert(tuple_equal(n4, vector(sqrt3_div3, sqrt3_div3, sqrt3_div3)));

        tuple_t n = shape_normal_at(
            &s, point(sqrt3_div3, sqrt3_div3, sqrt3_div3), NULL);
        assert(tuple_equal(n, tuple_normalize(n)));
    }

    { // Computing the normal on a translated sphere
        sphere_t s = sphere();
        shape_set_transform(&s, transform_translation(0, 1, 0));
        tuple_t n = shape_normal_at(&s, point(0, 1.70711, -0.70711), NULL);
        assert(tuple_equal(n, vector(0, 0.70711, -0.70711)));
    }

    { // Computing the normal on a transformed sphere
        sphere_t s = sphere();
        matrix_t m = matrix_mul(transform_scaling(1, 0.5, 1),
                                transform_rotation_z(M_PI_5));
        shape_set_transform(&s, m);
        tuple_t n = shape_normal_at(
            &s, point(0, sqrt(2.0) / 2.0, -sqrt(2.0) / 2.0), NULL);
        assert(tuple_equal(n, vector(0, 0.97014, -0.24254)));
    }

    { // A sphere has a default material
        sphere_t s   = sphere();
        material_t m = material();
        assert(tuple_equal(s.material.color, m.color));
        assert(equal(s.material.ambient, m.ambient));
        assert(equal(s.material.diffuse, m.diffuse));
        assert(equal(s.material.specular, m.specular));
        assert(equal(s.material.shininess, m.shininess));
    }

    { // A sphere may be assigned a material
        sphere_t s   = sphere();
        material_t m = material();
        m.ambient    = 1.0;
        s.material   = m;
        assert(tuple_equal(s.material.color, m.color));
        assert(equal(s.material.ambient, m.ambient));
        assert(equal(s.material.diffuse, m.diffuse));
        assert(equal(s.material.specular, m.specular));
        assert(equal(s.material.shininess, m.shininess));
    }

    { // A helper for producing a sphere with a glassy material

        sphere_t s = glass_sphere();
        assert(matrix_equal(s.transform, IDENTITY));
        assert(equal(s.material.transparency, 1.0));
        assert(equal(s.material.refractive_index, 1.5));
    }
}

int main(void)
{
    test_spheres();
    return 0;
}
