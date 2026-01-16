#ifndef VECTOR_H
#define VECTOR_H

#include "arena.h"
#include "string.h"  // Required for memcpy

// -----------------------------------------------------------------------------
// The "By Value" Vector Macro
// -----------------------------------------------------------------------------
// Usage:
//    ByteVector* vec = ARENA_ALLOC(ByteVector, 1);
//    Byte b = { .value = 10 };
//    vector_push(vec, b);
// -----------------------------------------------------------------------------

#define vector_push(vec, val)                                                              \
    do {                                                                                   \
        /* 1. Check Capacity */                                                            \
        if ((vec)->count >= (vec)->capacity) {                                             \
            size_t _new_cap = (vec)->capacity == 0 ? 16 : (vec)->capacity * 2;             \
                                                                                           \
            /* 2. Allocate New Block (using the type of the existing data) */              \
            /* typeof(*((vec)->data)) gives us the struct type (e.g., Byte) */             \
            typeof((vec)->data) _new_data = ARENA_ALLOC(typeof(*((vec)->data)), _new_cap); \
                                                                                           \
            /* 3. Copy Old Data (if any) */                                                \
            if ((vec)->count > 0) {                                                        \
                memcpy(_new_data, (vec)->data, (vec)->count * sizeof(*((vec)->data)));     \
            }                                                                              \
                                                                                           \
            /* 4. Update Vector (Old memory is left in Arena, no free needed) */           \
            (vec)->data = _new_data;                                                       \
            (vec)->capacity = _new_cap;                                                    \
        }                                                                                  \
                                                                                           \
        /* 5. Push Value */                                                                \
        (vec)->data[(vec)->count++] = (val);                                               \
    } while (0)

// -----------------------------------------------------------------------------
// The "By Value" Vector Append Macro
// -----------------------------------------------------------------------------
// Usage:
//    vector_append(tokens_vec, new_tokens_vec);
// -----------------------------------------------------------------------------

#define vector_append(dest, src)                                                                     \
    do {                                                                                             \
        /* 1. Calculate Required Size */                                                             \
        size_t _req_count = (dest)->count + (src).count;                                             \
                                                                                                     \
        /* 2. Check Capacity */                                                                      \
        if (_req_count > (dest)->capacity) {                                                         \
            size_t _new_cap = (dest)->capacity == 0 ? 16 : (dest)->capacity;                         \
            while (_new_cap < _req_count) _new_cap *= 2;                                             \
                                                                                                     \
            /* 3. Allocate New Block */                                                              \
            typeof((dest)->data) _new_data = ARENA_ALLOC(typeof(*((dest)->data)), _new_cap);         \
                                                                                                     \
            /* 4. Copy Old Data (if any) */                                                          \
            if ((dest)->count > 0) {                                                                 \
                memcpy(_new_data, (dest)->data, (dest)->count * sizeof(*((dest)->data)));            \
            }                                                                                        \
                                                                                                     \
            /* 5. Update Vector */                                                                   \
            (dest)->data = _new_data;                                                                \
            (dest)->capacity = _new_cap;                                                             \
        }                                                                                            \
                                                                                                     \
        /* 6. Append New Data */                                                                     \
        if ((src).count > 0) {                                                                       \
            memcpy((dest)->data + (dest)->count, (src).data, (src).count * sizeof(*((dest)->data))); \
            (dest)->count = _req_count;                                                              \
        }                                                                                            \
    } while (0)

#endif
