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
    // allocating memory for the handler itself
    mcc_core_arena* arena = malloc(_MCC_CORE_ARENA_ALIGN_UP(sizeof(mcc_core_arena)));
    if (!arena) {
        mcc_core_error_fatal("arena OOM: failed to allocate memory for arena handler\n");
    }

    // clip capacity
    capacity = MCC_CORE_UTILS_MAX(capacity, _MCC_CORE_ARENA_SIZE_DEFAULT);
    capacity = _MCC_CORE_ARENA_ALIGN_UP(capacity);

    // allocating memory for the handle
    arena->memory = malloc(capacity);
    if (!arena->memory) {
        mcc_core_error_fatal("arena OOM: failed to allocate %zu bytes for arena instance\n", capacity);
    }

    arena->capacity = capacity;
    arena->position = 0;
    arena->next = NULL;

    return arena;
}

void mcc_core_arena_destruct(mcc_core_arena* self) {
    // base case
    if (!self) {
        return;
    }

    mcc_core_arena_destruct(self->next);
    free(self->memory);
    free(self);
}

static bool _mcc_core_arena_available(mcc_core_arena* self, size_t size) {
    // align size
    size = _MCC_CORE_ARENA_ALIGN_UP(size);

    size_t available = self->capacity - self->position;

    return available >= size;
}

void* mcc_core_arena_allocate(mcc_core_arena* self, size_t size) {
    // align size
    size = _MCC_CORE_ARENA_ALIGN_UP(size);

    // check for availability in this arena (base case)
    if (_mcc_core_arena_available(self, size)) {
        uintptr_t memory = (uintptr_t)(self->memory) + self->position;
        self->position += size;

        // zero out the requested block
        void* ptr = (void*)memory;
        memset(ptr, 0, size);
        return ptr;
    }

    // if sufficient memory is not available in this arena, ask for it from next
    self->next = self->next ? self->next : mcc_core_arena_construct(size);
    return mcc_core_arena_allocate(self->next, size);
}

void mcc_core_arena_clear(mcc_core_arena* self) {
    // base case
    if (!self) {
        return;
    }

    mcc_core_arena_clear(self->next);
    self->position = 0;
}
