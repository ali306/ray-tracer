// sequences.c

#include "../include/sequences.h"
#include <string.h>

sequence_t sequence_new(void)
{
    sequence_t seq;
    seq.count         = 0;
    seq.current_index = 0;
    memset(seq.values, 0, sizeof(seq.values));
    return seq;
}

void sequence_add(sequence_t *seq, double value)
{
    if (seq->count < MAX_SEQUENCE_LENGTH)
    {
        seq->values[seq->count] = value;
        seq->count++;
    }
}

sequence_t sequence_from_array(const double *values, int count)
{
    sequence_t seq = sequence_new();
    for (int i = 0; i < count && i < MAX_SEQUENCE_LENGTH; i++)
    {
        sequence_add(&seq, values[i]);
    }
    return seq;
}

double sequence_next(sequence_t *seq)
{
    if (seq->count == 0)
    {
        return 0.5;
    }

    double value       = seq->values[seq->current_index];
    seq->current_index = (seq->current_index + 1) % seq->count;
    return value;
}

double sequence_at(const sequence_t *seq, int index)
{
    if (seq->count == 0)
    {
        return 0.5;
    }
    return seq->values[index % seq->count];
}