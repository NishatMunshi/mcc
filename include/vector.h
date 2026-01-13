#ifndef VECTOR_H
#define VECTOR_H

#include "arena.h"
#include "types.h"

// Uses explicit variable names to avoid collion
#define vector_push(vec, elem)                                                                 \
    do {                                                                                       \
        /* 1. Check if we need to grow */                                                      \
        if ((vec)->count >= (vec)->cap) {                                                      \
            /* 2. Calculate explicit sizes */                                                  \
            size_t _current_capacity = (vec)->cap;                                             \
            size_t _new_capacity = (_current_capacity == 0) ? 8 : (_current_capacity * 2);     \
            size_t _element_size = sizeof(*((vec)->data));                                     \
            size_t _allocation_size = _new_capacity * _element_size;                           \
                                                                                               \
            /* 3. Allocate fresh memory from the Arena */                                      \
            void* _new_data = arena_alloc(_allocation_size);                                   \
                                                                                               \
            /* 4. Copy old data manually (if it exists) */                                     \
            if ((vec)->data) {                                                                 \
                u8* _dest_ptr = (u8*)_new_data;                                                \
                u8* _src_ptr = (u8*)((vec)->data);                                             \
                size_t _total_copy_bytes = (vec)->count * _element_size;                       \
                                                                                               \
                for (size_t _byte_index = 0; _byte_index < _total_copy_bytes; _byte_index++) { \
                    _dest_ptr[_byte_index] = _src_ptr[_byte_index];                            \
                }                                                                              \
            }                                                                                  \
                                                                                               \
            /* 5. Update the vector structure */                                               \
            (vec)->data = _new_data;                                                           \
            (vec)->cap = _new_capacity;                                                        \
        }                                                                                      \
                                                                                               \
        /* 6. Push the element */                                                              \
        (vec)->data[(vec)->count++] = (elem);                                                  \
    } while (0)

#endif
