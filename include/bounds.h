// bounds.c

#include "shapes.h"
#include "tuples.h"
#include <float.h>

#ifndef BOUNDS_H
#define BOUNDS_H

bounding_box_t bounding_box_empty(void);
bounding_box_t bounding_box(tuple_t min, tuple_t max);
void bounds_add_point(bounding_box_t *box, tuple_t point);
bounding_box_t bounds_of(const void *shape_ptr);
bounding_box_t bounds_of_triangle(triangle_t triangle);
void bounds_add_box(bounding_box_t *box1, const bounding_box_t *box2);
bool bounds_box_contains_point(bounding_box_t box, tuple_t point);
bool bounds_box_contains_box(bounding_box_t box1, bounding_box_t box2);
bounding_box_t bounds_transform(bounding_box_t bbox, matrix_t matrix);
bounding_box_t bounds_parent_space_bounds_of(const shape_t *shape);
bounding_box_t bounds_of_group(const void *group_ptr);
bool bounds_intersects(bounding_box_t box, ray_t ray);
void split_bounds(bounding_box_t box, bounding_box_t *left,
                  bounding_box_t *right);
#endif
