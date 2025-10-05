#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdlib.h>

#define DYN_ARRAY_ENSURE_CAPACITY_IMPL(data_ptr, count, capacity,              \
                                       element_type, max_capacity)             \
    do                                                                         \
    {                                                                          \
        if ((count) >= (capacity))                                             \
        {                                                                      \
            unsigned new_cap = (capacity) == 0 ? 16 : (capacity) * 2;          \
            if (new_cap > (max_capacity))                                      \
            {                                                                  \
                new_cap = (max_capacity);                                      \
            }                                                                  \
            if ((count) >= new_cap)                                            \
            {                                                                  \
                return false;                                                  \
            }                                                                  \
            element_type *new_data =                                           \
                realloc((data_ptr), new_cap * sizeof(element_type));           \
            if (new_data == NULL)                                              \
            {                                                                  \
                return false;                                                  \
            }                                                                  \
            (data_ptr) = new_data;                                             \
            (capacity) = new_cap;                                              \
        }                                                                      \
    } while (0)

#endif