#ifndef MCC_CORE_VECTOR_H
#define MCC_CORE_VECTOR_H

#include "mcc/core/arena.h"

/**
 * @brief A dynamic array (growable list) allocated in the arena.
 * Stores void pointers (generic).
 */
typedef struct mcc_core_vector mcc_core_vector;

// Constructor
mcc_core_vector* mcc_core_vector_construct(mcc_core_arena* arena);

// Adds an item to the end
void mcc_core_vector_push(mcc_core_vector* self, void* item);

// Gets an item at index
void* mcc_core_vector_get(mcc_core_vector* self, size_t index);

// Returns number of items
size_t mcc_core_vector_size(mcc_core_vector* self);

#endif  // MCC_CORE_VECTOR_H