// patterns_tests.c

#include "../include/patterns.h"
#include "../include/shapes.h"
#include <assert.h>

void patterns_tests(void)
{
    { // Default pattern transformation
        pattern_t pattern = patterns_test();
        assert(matrix_equal(pattern.transform, IDENTITY));
    }

    { // Assigning a transformation to a pattern
        pattern_t pattern  = patterns_test();
        matrix_t transform = transform_translation(1, 2, 3);
        patterns_set_transform(&pattern, transform);
        assert(matrix_equal(pattern.transform, transform));
    }

    { // Pattern with object transformation
        shape_t shape = sphere();
        shape_set_transform(&shape, transform_scaling(2, 2, 2));
        pattern_t pattern = patterns_test();
        tuple_t c         = pattern_at_shape(pattern, shape, point(2, 3, 4));
        assert(tuple_equal(c, color(1, 1.5, 2)));
    }

    { // Pattern with pattern transformation
        shape_t shape     = sphere();
        pattern_t pattern = patterns_test();
        patterns_set_transform(&pattern, transform_scaling(2, 2, 2));
        tuple_t c = pattern_at_shape(pattern, shape, point(2, 3, 4));
        assert(tuple_equal(c, color(1, 1.5, 2)));
    }

    { // Pattern with both object and pattern transformations
        shape_t shape = sphere();
        shape_set_transform(&shape, transform_scaling(2, 2, 2));
        pattern_t pattern = patterns_test();
        patterns_set_transform(&pattern, transform_translation(0.5, 1, 1.5));
        tuple_t c = pattern_at_shape(pattern, shape, point(2.5, 3, 3.5));
        assert(tuple_equal(c, color(0.75, 0.5, 0.25)));
    }

    { // Creating a stripe pattern
        pattern_t pattern = patterns_stripe(WHITE, BLACK);
        assert(tuple_equal(pattern.a, WHITE));
        assert(tuple_equal(pattern.b, BLACK));
    }

    { // Stripe pattern is constant in y
        pattern_t pattern = patterns_stripe(WHITE, BLACK);
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0, 1, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0, 2, 0)), WHITE));
    }

    { // Stripe pattern is constant in z
        pattern_t pattern = patterns_stripe(WHITE, BLACK);
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 1)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 2)), WHITE));
    }

    { // Stripe pattern alternates in x
        pattern_t pattern = patterns_stripe(WHITE, BLACK);
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0.9, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(1, 0, 0)), BLACK));
        assert(tuple_equal(pattern_at(pattern, point(-0.1, 0, 0)), BLACK));
        assert(tuple_equal(pattern_at(pattern, point(-1, 0, 0)), BLACK));
        assert(tuple_equal(pattern_at(pattern, point(-1.1, 0, 0)), WHITE));
    }

    { // Gradient linearly interpolates between colors
        pattern_t pattern = patterns_gradient(WHITE, BLACK);
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0.25, 0, 0)),
                           color(0.75, 0.75, 0.75)));
        assert(tuple_equal(pattern_at(pattern, point(0.5, 0, 0)),
                           color(0.5, 0.5, 0.5)));
        assert(tuple_equal(pattern_at(pattern, point(0.75, 0, 0)),
                           color(0.25, 0.25, 0.25)));
    }

    { // Ring pattern extends in both x and z
        pattern_t pattern = patterns_ring(WHITE, BLACK);
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(1, 0, 0)), BLACK));
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 1)), BLACK));
        assert(tuple_equal(pattern_at(pattern, point(0.708, 0, 0.708)), BLACK));
    }

    { // Checkers pattern repeats in x, y, and z
        pattern_t pattern = patterns_checker(WHITE, BLACK);

        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0.99, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(1.01, 0, 0)), BLACK));

        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0, 0.99, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0, 1.01, 0)), BLACK));

        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 0.99)), WHITE));
        assert(tuple_equal(pattern_at(pattern, point(0, 0, 1.01)), BLACK));
    }
}

int main(void)
{
    patterns_tests();
    return 0;
}
