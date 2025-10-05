// test_bounds.c

#include "../include/bounds.h"
#include <assert.h>

void test_bounds(void)
{
    { // Creating an empty bounding box
        bounding_box_t box = bounding_box_empty();
        assert(tuple_equal(box.min, point(DBL_MAX, DBL_MAX, DBL_MAX)));
        assert(tuple_equal(box.max, point(-DBL_MAX, -DBL_MAX, -DBL_MAX)));
    }

    { // Creating a bounding box with volume
        bounding_box_t box = bounding_box(point(-1, -2, -3), point(3, 2, 1));
        assert(tuple_equal(box.min, point(-1, -2, -3)));
        assert(tuple_equal(box.max, point(3, 2, 1)));
    }

    { // Adding points to an empty bounding box
        bounding_box_t box = bounding_box_empty();
        tuple_t p1         = point(-5, 2, 0);
        tuple_t p2         = point(7, 0, -3);
        bounds_add_point(&box, p1);
        bounds_add_point(&box, p2);
        assert(tuple_equal(box.min, point(-5, 0, -3)));
        assert(tuple_equal(box.max, point(7, 2, 0)));
    }

    { // A sphere has a bounding box
        sphere_t shape     = sphere();
        bounding_box_t box = bounds_of(&shape);
        assert(tuple_equal(box.min, point(-1, -1, -1)));
        assert(tuple_equal(box.max, point(1, 1, 1)));
    }

    { // A plane has a bounding box
        plane_t shape      = plane();
        bounding_box_t box = bounds_of(&shape);
        assert(tuple_equal(box.min, point(-DBL_MAX, 0, -DBL_MAX)));
        assert(tuple_equal(box.max, point(DBL_MAX, 0, DBL_MAX)));
    }

    { // A cube has a bounding box
        cube_t shape       = cube();
        bounding_box_t box = bounds_of(&shape);
        assert(tuple_equal(box.min, point(-1, -1, -1)));
        assert(tuple_equal(box.max, point(1, 1, 1)));
    }

    { // A triangle has a bounding box
        triangle_t shape =
            triangle(point(-3, 7, 2), point(6, 2, -4), point(2, -1, -1));
        bounding_box_t box = bounds_of_triangle(shape);
        assert(tuple_equal(box.min, point(-3, -1, -4)));
        assert(tuple_equal(box.max, point(6, 7, 2)));
    }

    { // Test shape has (arbitrary) bounds
        shape_t s;
        shape(&s, SHAPE_TEST);
        bounding_box_t box = bounds_of(&s);
        assert(tuple_equal(box.min, point(-1, -1, -1)));
        assert(tuple_equal(box.max, point(1, 1, 1)));
    }

    { // Adding one bounding box to another
        bounding_box_t box1 = bounding_box(point(-5, -2, 0), point(7, 4, 4));
        bounding_box_t box2 = bounding_box(point(8, -7, -2), point(14, 2, 8));
        bounds_add_box(&box1, &box2);
        assert(tuple_equal(box1.min, point(-5, -7, -2)));
        assert(tuple_equal(box1.max, point(14, 4, 8)));
    }

    { // Checking to see if a box contains a given point
        bounding_box_t box = bounding_box(point(5, -2, 0), point(11, 4, 7));
        tuple_t p[9]       = {point(5, -2, 0), point(11, 4, 7), point(8, 1, 3),
                              point(3, 0, 3),  point(8, -4, 3), point(8, 1, -1),
                              point(13, 1, 3), point(8, 5, 3),  point(8, 1, 8)};
        bool expected[9]   = {true,  true,  true,  false, false,
                              false, false, false, false};
        for (int i = 0; i < 9; i++)
        {
            assert(bounds_box_contains_point(box, p[i]) == expected[i]);
        }
    }

    { // Checking to see if a box contains a given box
        bounding_box_t box = bounding_box(point(5, -2, 0), point(11, 4, 7));

        bounding_box_t box1 = bounding_box(point(5, -2, 0), point(11, 4, 7));
        assert(bounds_box_contains_box(box, box1) == true);

        bounding_box_t box2 = bounding_box(point(6, -1, 1), point(10, 3, 6));
        assert(bounds_box_contains_box(box, box2) == true);

        bounding_box_t box3 = bounding_box(point(4, -3, -1), point(10, 3, 6));
        assert(bounds_box_contains_box(box, box3) == false);

        bounding_box_t box4 = bounding_box(point(6, -1, 1), point(12, 5, 8));
        assert(bounds_box_contains_box(box, box4) == false);
    }

    { // Transforming a bounding box
        bounding_box_t box  = bounding_box(point(-1, -1, -1), point(1, 1, 1));
        matrix_t matrix     = matrix_mul(transform_rotation_x(M_PI_4),
                                         transform_rotation_y(M_PI_4));
        bounding_box_t box2 = bounds_transform(box, matrix);
        assert(tuple_equal(box2.min, point(-1.4142, -1.7071, -1.7071)));
        assert(tuple_equal(box2.max, point(1.4142, 1.7071, 1.7071)));
    }

    { // Querying a shape's bounding box in its parent's space
        sphere_t shape = sphere();
        sphere_set_transform(&shape, matrix_mul(transform_translation(1, -3, 5),
                                                transform_scaling(0.5, 2, 4)));
        bounding_box_t box = bounds_parent_space_bounds_of(&shape);
        assert(tuple_equal(box.min, point(0.5, -5, 1)));
        assert(tuple_equal(box.max, point(1.5, -1, 9)));
    }

    { // A group has a bounding box that contains its children
        sphere_t s = sphere();
        sphere_set_transform(&s, matrix_mul(transform_translation(2, 5, -3),
                                            transform_scaling(2, 2, 2)));

        cylinder_t c = cylinder();
        c.minimum    = -2;
        c.maximum    = 2;
        shape_set_transform((shape_t *)&c, matrix_mul(transform_translation(-4, -1, 4),
                                           transform_scaling(0.5, 1, 0.5)));

        group_t *shape     = group();
        sphere_t *s_heap   = malloc(sizeof(sphere_t));
        cylinder_t *c_heap = malloc(sizeof(cylinder_t));
        *s_heap            = s;
        *c_heap            = c;

        group_add_child(shape, (shape_t *)s_heap);
        group_add_child(shape, (shape_t *)c_heap);

        bounding_box_t box = bounds_of_group(shape);
        assert(tuple_equal(box.min, point(-4.5, -3, -5)));
        assert(tuple_equal(box.max, point(4, 7, 4.5)));

        group_free(shape);
    }

    { // Intersecting a ray with a bounding box at the origin
        bounding_box_t box = bounding_box(point(-1, -1, -1), point(1, 1, 1));

        struct
        {
            tuple_t origin;
            tuple_t direction;
            bool expected;
        } test_cases[] = {
            {point(5, 0.5, 0), tuple_normalize(vector(-1, 0, 0)), true},
            {point(-5, 0.5, 0), tuple_normalize(vector(1, 0, 0)), true},
            {point(0.5, 5, 0), tuple_normalize(vector(0, -1, 0)), true},
            {point(0.5, -5, 0), tuple_normalize(vector(0, 1, 0)), true},
            {point(0.5, 0, 5), tuple_normalize(vector(0, 0, -1)), true},
            {point(0.5, 0, -5), tuple_normalize(vector(0, 0, 1)), true},
            {point(0, 0.5, 0), tuple_normalize(vector(0, 0, 1)), true},
            {point(-2, 0, 0), tuple_normalize(vector(2, 4, 6)), false},
            {point(0, -2, 0), tuple_normalize(vector(6, 2, 4)), false},
            {point(0, 0, -2), tuple_normalize(vector(4, 6, 2)), false},
            {point(2, 0, 2), tuple_normalize(vector(0, 0, -1)), false},
            {point(0, 2, 2), tuple_normalize(vector(0, -1, 0)), false},
            {point(2, 2, 0), tuple_normalize(vector(-1, 0, 0)), false}};

        for (int i = 0; i < 13; i++)
        {
            ray_t r     = ray(test_cases[i].origin, test_cases[i].direction);
            bool result = bounds_intersects(box, r);
            assert(result == test_cases[i].expected);
        }
    }

    { // Intersecting a ray with a non-cubic bounding box
        bounding_box_t box = bounding_box(point(5, -2, 0), point(11, 4, 7));

        struct
        {
            tuple_t origin;
            tuple_t direction;
            bool expected;
        } test_cases[] = {
            {point(15, 1, 2), tuple_normalize(vector(-1, 0, 0)), true},
            {point(-5, -1, 4), tuple_normalize(vector(1, 0, 0)), true},
            {point(7, 6, 5), tuple_normalize(vector(0, -1, 0)), true},
            {point(9, -5, 6), tuple_normalize(vector(0, 1, 0)), true},
            {point(8, 2, 12), tuple_normalize(vector(0, 0, -1)), true},
            {point(6, 0, -5), tuple_normalize(vector(0, 0, 1)), true},
            {point(8, 1, 3.5), tuple_normalize(vector(0, 0, 1)), true},
            {point(9, -1, -8), tuple_normalize(vector(2, 4, 6)), false},
            {point(8, 3, -4), tuple_normalize(vector(6, 2, 4)), false},
            {point(9, -1, -2), tuple_normalize(vector(4, 6, 2)), false},
            {point(4, 0, 9), tuple_normalize(vector(0, 0, -1)), false},
            {point(8, 6, -1), tuple_normalize(vector(0, -1, 0)), false},
            {point(12, 5, 4), tuple_normalize(vector(-1, 0, 0)), false}};

        for (int i = 0; i < 13; i++)
        {
            ray_t r     = ray(test_cases[i].origin, test_cases[i].direction);
            bool result = bounds_intersects(box, r);
            assert(result == test_cases[i].expected);
        }
    }

    { // Splitting a perfect cube
        bounding_box_t box = bounding_box(point(-1, -4, -5), point(9, 6, 5));
        bounding_box_t left, right;
        split_bounds(box, &left, &right);

        assert(tuple_equal(left.min, point(-1, -4, -5)));
        assert(tuple_equal(left.max, point(4, 6, 5)));
        assert(tuple_equal(right.min, point(4, -4, -5)));
        assert(tuple_equal(right.max, point(9, 6, 5)));
    }

    { // Splitting an x-wide box
        bounding_box_t box = bounding_box(point(-1, -2, -3), point(9, 5.5, 3));
        bounding_box_t left, right;
        split_bounds(box, &left, &right);

        assert(tuple_equal(left.min, point(-1, -2, -3)));
        assert(tuple_equal(left.max, point(4, 5.5, 3)));
        assert(tuple_equal(right.min, point(4, -2, -3)));
        assert(tuple_equal(right.max, point(9, 5.5, 3)));
    }

    { // Splitting a y-wide box
        bounding_box_t box = bounding_box(point(-1, -2, -3), point(5, 8, 3));
        bounding_box_t left, right;
        split_bounds(box, &left, &right);

        assert(tuple_equal(left.min, point(-1, -2, -3)));
        assert(tuple_equal(left.max, point(5, 3, 3)));
        assert(tuple_equal(right.min, point(-1, 3, -3)));
        assert(tuple_equal(right.max, point(5, 8, 3)));
    }

    { // Splitting a z-wide box
        bounding_box_t box = bounding_box(point(-1, -2, -3), point(5, 3, 7));
        bounding_box_t left, right;
        split_bounds(box, &left, &right);

        assert(tuple_equal(left.min, point(-1, -2, -3)));
        assert(tuple_equal(left.max, point(5, 3, 2)));
        assert(tuple_equal(right.min, point(-1, -2, 2)));
        assert(tuple_equal(right.max, point(5, 3, 7)));
    }

    { // An unbounded cylinder has a bounding box
        cylinder_t shape   = cylinder();
        bounding_box_t box = bounds_of(&shape);
        assert(tuple_equal(box.min, point(-1, -DBL_MAX, -1)));
        assert(tuple_equal(box.max, point(1, DBL_MAX, 1)));
    }

    { // A bounded cylinder has a bounding box
        cylinder_t shape = cylinder();
        shape.minimum    = -5.0;
        shape.maximum    = 3.0;
        bounding_box_t box = bounds_of(&shape);
        assert(tuple_equal(box.min, point(-1, -5, -1)));
        assert(tuple_equal(box.max, point(1, 3, 1)));
    }

    { // An unbounded cone has a bounding box
        cone_t shape       = cone();
        bounding_box_t box = bounds_of(&shape);
        assert(tuple_equal(box.min, point(-DBL_MAX, -DBL_MAX, -DBL_MAX)));
        assert(tuple_equal(box.max, point(DBL_MAX, DBL_MAX, DBL_MAX)));
    }

    { // A bounded cone has a bounding box
        cone_t shape   = cone();
        shape.minimum  = -5.0;
        shape.maximum  = 3.0;
        bounding_box_t box = bounds_of(&shape);
        assert(tuple_equal(box.min, point(-5, -5, -5)));
        assert(tuple_equal(box.max, point(5, 3, 5)));
    }
}

int main(void)
{
    test_bounds();
    return 0;
}
