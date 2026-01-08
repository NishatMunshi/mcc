#include "mcc/core/vector.h"

#include <stdint.h>
#include <string.h>

#include "mcc/core/error.h"

#define _MCC_CORE_VECTOR_DEFAULT_CAPACITY 8
#define _MCC_CORE_VECTOR_ITEM_SIZE (sizeof(void*))

struct mcc_core_vector {
    mcc_core_arena* arena;
    size_t capacity;
    size_t count;
    void** data;  // Array of pointers
};

mcc_core_vector* mcc_core_vector_construct(mcc_core_arena* arena) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(arena);

    mcc_core_vector* self = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_vector, 1);

    self->arena = arena;
    self->capacity = _MCC_CORE_VECTOR_DEFAULT_CAPACITY;
    self->count = 0;
    // Allocating an array of pointers (void*)
    self->data = mcc_core_arena_allocate(arena, self->capacity * _MCC_CORE_VECTOR_ITEM_SIZE);

    return self;
}

static void _mcc_core_vector_resize(mcc_core_vector* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    size_t new_cap = 2 * self->capacity;
    void** new_mem = mcc_core_arena_allocate(self->arena, new_cap * _MCC_CORE_VECTOR_ITEM_SIZE);

    // Copy only the valid existing pointers
    // Since data is void**, we can just multiply count * sizeof(void*)
    memcpy(new_mem, self->data, self->count * _MCC_CORE_VECTOR_ITEM_SIZE);

    self->data = new_mem;
    self->capacity = new_cap;
}

void mcc_core_vector_push(mcc_core_vector* self, void* item) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);
    // Note: item CAN be NULL (e.g. if you want to store a null pointer in the list)

    if (self->count >= self->capacity) {
        _mcc_core_vector_resize(self);
    }

    // SIMPLE ASSIGNMENT:
    // Because data is void**, C knows that data[i] is a void*.
    // We just store the pointer value directly. No memcpy needed.
    self->data[self->count] = item;

    self->count++;
}

void* mcc_core_vector_get(mcc_core_vector* self, size_t index) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    if (index >= self->count) {
        mcc_core_error_fatal("%s: vector index out of bounds: %zu", __func__, index);
    }

    return self->data[index];
}

size_t mcc_core_vector_size(mcc_core_vector* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    return self->count;
}