#include "mcc/core/arena.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc/core/error.h"

#define _MCC_CORE_ARENA_SIZE_DEFAULT (MCC_CORE_UTILS_MiB(1))
#define _MCC_CORE_ARENA_ALIGNMENT_BYTES (_Alignof(max_align_t))
#define _MCC_CORE_ARENA_ALIGN_UP(number) (((number) + _MCC_CORE_ARENA_ALIGNMENT_BYTES - 1) & ~(_MCC_CORE_ARENA_ALIGNMENT_BYTES - 1))

struct mcc_core_arena {
    void* memory;
    size_t capacity;
    size_t position;
    mcc_core_arena* next;
};

mcc_core_arena* mcc_core_arena_construct(size_t capacity) {
    if (capacity == 0) {
        mcc_core_error_fatal("mcc_core_arena_construct: arena of capacity zero requested");
    }

    // allocating memory for the handler itself
    mcc_core_arena* arena = malloc(_MCC_CORE_ARENA_ALIGN_UP(sizeof(mcc_core_arena)));
    if (arena == NULL) {
        mcc_core_error_fatal("mcc_core_arena_construct: failed to allocate memory for arena handler of requested capacity = %zu\n", capacity);
    }

    // clip capacity
    size_t capacity_clipped = MCC_CORE_UTILS_MAX(capacity, _MCC_CORE_ARENA_SIZE_DEFAULT);
    size_t capacity_aligned = _MCC_CORE_ARENA_ALIGN_UP(capacity_clipped);

    // allocating memory for the handle
    arena->memory = malloc(capacity_aligned);
    if (arena->memory == NULL) {
        mcc_core_error_fatal("mcc_core_arena_construct: failed to allocate %zu bytes for arena instance\n", capacity_aligned);
    }

    arena->capacity = capacity_aligned;
    arena->position = 0;
    arena->next = NULL;

    return arena;
}

void mcc_core_arena_destruct(mcc_core_arena* self) {
    // base case
    if (self == NULL) {
        return;
    }

    mcc_core_arena_destruct(self->next);
    free(self->memory);
    free(self);
}

static bool _mcc_core_arena_available(mcc_core_arena* self, size_t size) {
    if (self == NULL) {
        mcc_core_error_fatal("_mcc_core_arena_available: NULL passed as self");
    }

    if (size == 0) {
        mcc_core_error_fatal("_mcc_core_arena_available: allocation of zero size requested");
    }

    // align size
    size_t size_aligned = _MCC_CORE_ARENA_ALIGN_UP(size);

    size_t available = self->capacity - self->position;

    return available >= size_aligned;
}

void* mcc_core_arena_allocate(mcc_core_arena* self, size_t size) {
    if (self == NULL) {
        mcc_core_error_fatal("mcc_core_arena_allocate: NULL passed as self");
    }

    if (size == 0) {
        mcc_core_error_fatal("mcc_core_arena_allocate: allocation of zero size requested");
    }

    // align size
    size_t size_aligned = _MCC_CORE_ARENA_ALIGN_UP(size);

    // check for availability in this arena (base case)
    if (_mcc_core_arena_available(self, size_aligned)) {
        // we use uintptr_t for ISO C pointer arithmatic
        uintptr_t memory = (uintptr_t)(self->memory) + self->position;
        self->position += size_aligned;

        // zero out the requested block
        void* ptr = (void*)memory;
        memset(ptr, 0, size_aligned);
        return ptr;
    }

    // if sufficient memory is not available in this arena, ask for it from next
    // if next does not exist, create it
    // we do not fail here, only fail happens if malloc fails
    self->next = self->next ? self->next : mcc_core_arena_construct(size_aligned);
    return mcc_core_arena_allocate(self->next, size_aligned);
}

void mcc_core_arena_clear(mcc_core_arena* self) {
    // base case
    if (self == NULL) {
        return;
    }

    mcc_core_arena_clear(self->next);
    self->position = 0;
}
