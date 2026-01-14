#ifndef VECTOR_H
#define VECTOR_H

#include "arena.h"
#include "types.h"

// Uses explicit variable names to avoid collion
#define vector_push(vec, item)                                                           \
    do {                                                                                 \
        if ((vec)->capacity == 0) {                                                      \
            (vec)->capacity = 32;                                                        \
            /* 1. Allocate based on the TYPE pointed to by data */                       \
            (vec)->data = ARENA_ALLOC(typeof(*((vec)->data)), (vec)->capacity);          \
        } else if ((vec)->count >= (vec)->capacity) {                                    \
            size_t new_cap = (vec)->capacity * 2;                                        \
            /* 2. Declare new pointer with the EXACT type of vec->data */                \
            typeof((vec)->data) new_data = ARENA_ALLOC(typeof(*((vec)->data)), new_cap); \
                                                                                         \
            for (size_t i = 0; i < (vec)->count; i++) {                                  \
                new_data[i] = (vec)->data[i];                                            \
            }                                                                            \
                                                                                         \
            (vec)->data = new_data;                                                      \
            (vec)->capacity = new_cap;                                                   \
        }                                                                                \
        (vec)->data[(vec)->count++] = (item);                                            \
    } while (0)

#endif  // VECTOR_H
