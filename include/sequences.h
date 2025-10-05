// sequences.h

#ifndef SEQUENCES_H
#define SEQUENCES_H

#include "config.h"

typedef struct
{
    double values[MAX_SEQUENCE_LENGTH];
    int count;
    int current_index;
} sequence_t;

sequence_t sequence_new(void);
void sequence_add(sequence_t *seq, double value);
sequence_t sequence_from_array(const double *values, int count);
double sequence_next(sequence_t *seq);
double sequence_at(const sequence_t *seq, int index);

#endif