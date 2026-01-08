#include "mcc/core/vector.h"
#include "mcc/core/error.h"

struct mcc_core_vector {
    mcc_core_arena *arena;  // <--- Needed for resizing
    void *data;             // Contiguous block of memory
    size_t item_size;       // Size of one element in bytes
    size_t capacity;        // How many items fits in 'data'
    size_t count;           // Current number of items
};

mcc_core_vector *mcc_core_vector_construct(mcc_core_arena *arena, size_t item_size) {
    MCC_CORE_ERROR_CHECK_NULL(arena);

    mcc_core_vector *self = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_vector, 1);

    

    
}

// Adds an item to the end
void mcc_core_vector_push(mcc_core_vector *self, void *item);

// Gets an item at index
void *mcc_core_vector_get(mcc_core_vector *self, size_t index);

// Returns number of items
size_t mcc_core_vector_size(mcc_core_vector *self);
