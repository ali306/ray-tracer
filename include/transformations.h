// transformations.h

#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "../include/matrices.h"
#include "../include/tuples.h"
#include <stdbool.h>

matrix_t transform_translation(const double x, const double y, const double z);

matrix_t transform_scaling(const double x, const double y, const double z);

matrix_t transform_rotation_x(double const radians);

matrix_t transform_rotation_y(double const radians);

matrix_t transform_rotation_z(double const radians);

matrix_t transform_shearing(const double x_y, const double x_z,
                            const double y_x, const double y_z,
                            const double z_x, const double z_y);

matrix_t transform_view(const tuple_t from, const tuple_t to, const tuple_t up);

#endif
