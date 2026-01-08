#include "mcc/core/arena.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>  // for memset

#include "mcc/core/error.h"

/* --- Configuration Macros --- */

#define _MCC_CORE_ARENA_SIZE_DEFAULT (MCC_CORE_UTILS_MiB(1))
#define _MCC_CORE_ARENA_ALIGNMENT_BYTES (_Alignof(max_align_t))

#define _MCC_CORE_ARENA_ALIGN_UP(number) \
    (((number) + _MCC_CORE_ARENA_ALIGNMENT_BYTES - 1) & ~(_MCC_CORE_ARENA_ALIGNMENT_BYTES - 1))

/* --- Internal Structures --- */

typedef struct _mcc_arena_block {
    struct _mcc_arena_block* next;
    size_t capacity;
    size_t pos;
    _Alignas(max_align_t) uint8_t data[];
} _mcc_arena_block;

struct mcc_core_arena {
    _mcc_arena_block* first;
    _mcc_arena_block* current;
};

/* --- Private Helper Functions --- */

/**
 * @brief Allocates a raw new block from the OS.
 */
static _mcc_arena_block* _mcc_core_arena_block_create(size_t capacity) {
    MCC_CORE_ERROR_FATAL_CHECK_ZERO(capacity);

    size_t total_size = sizeof(_mcc_arena_block) + capacity;
    _mcc_arena_block* block = (_mcc_arena_block*)malloc(total_size);

    if (block == NULL) {
        mcc_core_error_fatal("%s: Failed to allocate block of size %zu", __func__, total_size);
    }

    block->next = NULL;
    block->capacity = capacity;
    block->pos = 0;
    return block;
}

/**
 * @brief Attempts to allocate from a specific block.
 * * @return Pointer to memory if successful, NULL if the block is full.
 */
static void* _mcc_core_arena_attempt_alloc(_mcc_arena_block* block, size_t size) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(block);
    MCC_CORE_ERROR_FATAL_CHECK_ZERO(size);

    // Explicit Bound Check: The check happens right here, before we touch memory.
    if (block->pos + size > block->capacity) {
        return NULL;
    }

    void* ptr = block->data + block->pos;
    block->pos += size;
    memset(ptr, 0, size);
    return ptr;
}

/**
 * @brief Ensures the arena has a 'current' block capable of fitting 'needed_size'.
 * * This either:
 * 1. Reuses the existing next block if it fits.
 * 2. Allocates a new block and inserts it into the chain.
 */
static void _mcc_core_arena_grow(mcc_core_arena* self, size_t needed_size) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);
    MCC_CORE_ERROR_FATAL_CHECK_ZERO(needed_size);

    _mcc_arena_block* cur = self->current;

    // Strategy 1: Check if we have a 'next' block from a previous reuse
    if (cur->next != NULL) {
        _mcc_arena_block* next = cur->next;

        // We must strictly check if the reused block is big enough.
        if (next->capacity >= needed_size) {
            next->pos = 0;         // Reset it
            self->current = next;  // Use it
            return;
        }

        // If 'next' exists but is too small, we cannot use it.
        // We fall through to Strategy 2 (Insert a new larger block).
    }

    // Strategy 2: Allocate a new block
    size_t next_capacity = MCC_CORE_UTILS_MAX(_MCC_CORE_ARENA_SIZE_DEFAULT, needed_size);
    _mcc_arena_block* new_block = _mcc_core_arena_block_create(next_capacity);

    //
    // Insert the new block into the chain.
    // usage: A (cur) -> B (next, too small)
    // becomes: A (cur) -> New -> B
    // This preserves B for potential future use or correct destruction.
    new_block->next = cur->next;
    cur->next = new_block;

    self->current = new_block;
}

/* --- Public Implementation --- */

mcc_core_arena* mcc_core_arena_construct() {
    mcc_core_arena* self = (mcc_core_arena*)malloc(sizeof(mcc_core_arena));
    if (self == NULL) {
        mcc_core_error_fatal("%s: failed to allocate arena handle", __func__);
    }

    _mcc_arena_block* first = _mcc_core_arena_block_create(_MCC_CORE_ARENA_SIZE_DEFAULT);
    self->first = first;
    self->current = first;

    return self;
}

void mcc_core_arena_destruct(mcc_core_arena* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    _mcc_arena_block* it = self->first;
    while (it != NULL) {
        _mcc_arena_block* next = it->next;
        free(it);
        it = next;
    }
    free(self);
}

void* mcc_core_arena_allocate(mcc_core_arena* self, size_t size) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);
    MCC_CORE_ERROR_FATAL_CHECK_ZERO(size);

    size_t aligned_size = _MCC_CORE_ARENA_ALIGN_UP(size);

    // 1. Try to allocate from the current block
    // The check is performed inside '_attempt_alloc'.
    void* ptr = _mcc_core_arena_attempt_alloc(self->current, aligned_size);

    // 2. Success? Return immediately.
    if (ptr != NULL) {
        return ptr;
    }

    // 3. Failure? Grow the arena.
    // This guarantees 'self->current' will point to a block with sufficient capacity.
    _mcc_core_arena_grow(self, aligned_size);

    // 4. Retry allocation
    ptr = _mcc_core_arena_attempt_alloc(self->current, aligned_size);

    // 5. Sanity Check
    // If logic is correct, this is unreachable.
    // If we crash here, '_grow' failed to provide a valid block.
    if (ptr == NULL) {
        mcc_core_error_fatal("%s: allocation failed after growth", __func__);
    }

    return ptr;
}

void mcc_core_arena_clear(mcc_core_arena* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    self->current = self->first;
    self->first->pos = 0;

    // We do not need to clear the whole chain
    // We only clear a block when we are gonna use it
}