// canvas.c

#include "../include/canvas.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

canvas_t *canvas(unsigned width, unsigned height)
{
    if (width == 0 || height == 0)
        return NULL;

    canvas_t *c = malloc(sizeof(canvas_t));
    if (c == NULL)
        return NULL;

    c->width  = width;
    c->height = height;
    c->pixels = calloc(width * height, sizeof(tuple_t));

    if (c->pixels == NULL)
    {
        free(c);
        return NULL;
    }

    return c;
}

void canvas_write_pixel(canvas_t *c, unsigned x, unsigned y, tuple_t color)
{
    if (c == NULL || x >= c->width || y >= c->height)
    {
        return;
    }

    tuple_t *pixel = &c->pixels[y * c->width + x];
    pixel->x       = canvas_clamp(color.x);
    pixel->y       = canvas_clamp(color.y);
    pixel->z       = canvas_clamp(color.z);
    pixel->w       = canvas_clamp(color.w);
}

tuple_t canvas_pixel_at(const canvas_t *c, unsigned x, unsigned y)
{
    if (c == NULL || x >= c->width || y >= c->height)
    {
        return color(0, 0, 0);
    }

    return c->pixels[y * c->width + x];
}

char *canvas_to_ppm(const canvas_t *c)
{
    if (!c || c->width <= 0 || c->height <= 0)
        return NULL;

    size_t size = 32 + (12ULL * c->width * c->height);
    if (size < (12ULL * c->width * c->height))
        return NULL;
    char *buffer = malloc(size);
    if (!buffer)
        return NULL;

    int pos = sprintf(buffer, "P3\n%u %u\n255\n", c->width, c->height);

    for (unsigned y = 0; y < c->height; y++)
    {
        for (unsigned x = 0; x < c->width; x++)
        {
            tuple_t pixel = c->pixels[y * c->width + x];

            int r = (int)round(255.0 * canvas_clamp(pixel.x));
            int g = (int)round(255.0 * canvas_clamp(pixel.y));
            int b = (int)round(255.0 * canvas_clamp(pixel.z));

            pos += sprintf(buffer + pos, "%d %d %d ", r, g, b);
        }

        buffer[pos - 1] = '\n';
    }

    return buffer;
}

void canvas_free(canvas_t *c)
{
    if (c)
    {
        free(c->pixels);
        free(c);
    }
}

bool canvas_save(const canvas_t *c, const char *file_path)
{
    if (!c || !file_path)
    {
        fprintf(stderr, "Invalid parameters for canvas_save\n");
        return false;
    }

    FILE *file = fopen(file_path, "wb");
    if (!file)
    {
        fprintf(stderr, "Failed to open %s for writing (%s)\n", file_path,
                strerror(errno));
        return false;
    }

    fprintf(file, "P3\n%u %u\n255\n", c->width, c->height);

    char *line_buffer = malloc(c->width * 12 + 2);
    if (!line_buffer)
    {
        fclose(file);
        return false;
    }

    for (unsigned y = 0; y < c->height; y++)
    {
        int line_pos = 0;

        for (unsigned x = 0; x < c->width; x++)
        {
            tuple_t pixel = c->pixels[y * c->width + x];

            int r = (int)round(255.0 * canvas_clamp(pixel.x));
            int g = (int)round(255.0 * canvas_clamp(pixel.y));
            int b = (int)round(255.0 * canvas_clamp(pixel.z));

            line_pos += sprintf(line_buffer + line_pos, "%d %d %d ", r, g, b);
        }

        line_buffer[line_pos - 1] = '\n';
        line_buffer[line_pos]     = '\0';

        fputs(line_buffer, file);
    }

    free(line_buffer);
    fclose(file);

    return true;
}
