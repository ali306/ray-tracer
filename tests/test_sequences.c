// test_sequences.c

#include "../include/sequences.h"
#include "../include/tuples.h"
#include <assert.h>
#include <stdio.h>

void test_sequences(void)
{
    { // A number generator returns a cyclic sequence of numbers
        sequence_t gen = sequence_new();
        sequence_add(&gen, 0.1);
        sequence_add(&gen, 0.5);
        sequence_add(&gen, 1.0);

        assert(equal(sequence_next(&gen), 0.1));
        assert(equal(sequence_next(&gen), 0.5));
        assert(equal(sequence_next(&gen), 1.0));
        assert(equal(sequence_next(&gen), 0.1));
    }

    { // Creating a sequence from an array
        double values[] = {0.3, 0.7, 0.9};
        sequence_t seq  = sequence_from_array(values, 3);

        assert(equal(sequence_next(&seq), 0.3));
        assert(equal(sequence_next(&seq), 0.7));
        assert(equal(sequence_next(&seq), 0.9));
        assert(equal(sequence_next(&seq), 0.3));
    }
}

int main(void)
{
    test_sequences();
    return 0;
}