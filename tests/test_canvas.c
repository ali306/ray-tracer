// test_canvas.c

#include "../include/canvas.h"
#include "../include/tuples.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_canvas(void)
{
    { // Creating a canvas
        canvas_t *c = canvas(10, 20);
        assert(c->width == 10);
        assert(c->height == 20);
        for (int i = 0; i < 10 * 20; i++)
        {
            assert(equal(c->pixels[i].x, 0) && equal(c->pixels[i].y, 0) &&
                   equal(c->pixels[i].z, 0));
        }

        canvas_free(c);
    }

    { // Writing pixels to a canvas
        canvas_t *c = canvas(10, 20);
        tuple_t red = color(1, 0, 0);
        canvas_write_pixel(c, 2, 3, red);
        assert(tuple_equal(canvas_pixel_at(c, 2, 3), red));
        canvas_free(c);
    }

    { // Constructing the PPM header
        canvas_t *c    = canvas(5, 3);
        char *ppm      = canvas_to_ppm(c);
        char *expected = "P3\n5 3\n255\n";
        assert(strncmp(ppm, expected, strlen(expected)) == 0);
        free(ppm);
        canvas_free(c);
    }

    { // Constructing the PPM pixel data
        canvas_t *c = canvas(5, 3);
        tuple_t c1  = color(1.5, 0, 0);
        tuple_t c2  = color(0, 0.5, 0);
        tuple_t c3  = color(-0.5, 0, 1);
        canvas_write_pixel(c, 0, 0, c1);
        canvas_write_pixel(c, 2, 1, c2);
        canvas_write_pixel(c, 4, 2, c3);
        char *ppm      = canvas_to_ppm(c);
        char *expected = "P3\n"
                         "5 3\n"
                         "255\n"
                         "255 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n"
                         "0 0 0 0 0 0 0 128 0 0 0 0 0 0 0\n"
                         "0 0 0 0 0 0 0 0 0 0 0 0 0 0 255\n";
        assert(strcmp(ppm, expected) == 0);
        free(ppm);
        canvas_free(c);
    }
}

int main(void)
{
    test_canvas();

    return 0;
}
