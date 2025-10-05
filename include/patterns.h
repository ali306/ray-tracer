// patterns.h

#ifndef PATTERNS_H
#define PATTERNS_H

#include "config.h"
#include "transformations.h"
#include "tuples.h"

typedef enum
{
    PATTERN_TEST,
    PATTERN_STRIPE,
    PATTERN_GRADIENT,
    PATTERN_RING,
    PATTERN_CHECKER
} pattern_type_t;

typedef struct
{
    pattern_type_t type;
    matrix_t transform;
    matrix_t inverse_transform;
    tuple_t a;
    tuple_t b;
} pattern_t;

pattern_t pattern(const pattern_type_t type);
void patterns_set_transform(pattern_t *p, const matrix_t t);
tuple_t pattern_at(const pattern_t p, const tuple_t point);

// Test
pattern_t patterns_test(void);

// Stripe
pattern_t patterns_stripe(const tuple_t a, const tuple_t b);

// Gradient
pattern_t patterns_gradient(tuple_t a, tuple_t b);

// Ring
pattern_t patterns_ring(const tuple_t a, const tuple_t b);

// Checker
pattern_t patterns_checker(const tuple_t a, const tuple_t b);

#endif
