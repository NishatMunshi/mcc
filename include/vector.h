#ifndef VECTOR_H
#define VECTOR_H

#include "arena.h"
#include "string.h" // Required for memcpy

// -----------------------------------------------------------------------------
// The "By Value" Vector Macro
// -----------------------------------------------------------------------------
// Usage:
//    ByteVector* vec = ARENA_ALLOC(ByteVector, 1);
//    Byte b = { .value = 10 };
//    vector_push(vec, b);
// -----------------------------------------------------------------------------

#define vector_push(vec, val) do { \
    /* 1. Check Capacity */ \
    if ((vec)->count >= (vec)->capacity) { \
        size_t _new_cap = (vec)->capacity == 0 ? 16 : (vec)->capacity * 2; \
        \
        /* 2. Allocate New Block (using the type of the existing data) */ \
        /* typeof(*((vec)->data)) gives us the struct type (e.g., Byte) */ \
        typeof((vec)->data) _new_data = ARENA_ALLOC(typeof(*((vec)->data)), _new_cap); \
        \
        /* 3. Copy Old Data (if any) */ \
        if ((vec)->count > 0) { \
            memcpy(_new_data, (vec)->data, (vec)->count * sizeof(*((vec)->data))); \
        } \
        \
        /* 4. Update Vector (Old memory is left in Arena, no free needed) */ \
        (vec)->data = _new_data; \
        (vec)->capacity = _new_cap; \
    } \
    \
    /* 5. Push Value */ \
    (vec)->data[(vec)->count++] = (val); \
} while(0)

#endif
