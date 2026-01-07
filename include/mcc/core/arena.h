#ifndef MCC_CORE_ARENA_H
#define MCC_CORE_ARENA_H

#include <stddef.h>

#include "mcc/core/utils.h"

typedef struct mcc_core_arena mcc_core_arena;

// constructor
mcc_core_arena *mcc_core_arena_construct();

// destructor
void mcc_core_arena_destruct(mcc_core_arena *self);

// allocators
void *mcc_core_arena_allocate(mcc_core_arena *self, size_t size);
#define MCC_CORE_ARENA_ALLOCATE(arena, type, count) \
    (type *)mcc_core_arena_allocate((arena), (count * sizeof(type)))

// modifiers
void mcc_core_arena_clear(mcc_core_arena *self);

#endif  // MCC_CORE_ARENA_H