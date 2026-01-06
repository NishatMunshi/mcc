#ifndef MCC_CORE_ARENA_H
#define MCC_CORE_ARENA_H

#include <stddef.h>

#include "mcc/core/utils.h"

typedef struct mcc_core_arena mcc_core_arena;

mcc_core_arena *mcc_core_arena_construct();
void mcc_core_arena_destruct(mcc_core_arena *arena);
void *mcc_core_arena_allocate(mcc_core_arena *arena, size_t size);
void mcc_core_arena_clear(mcc_core_arena *arena);

#endif  // MCC_CORE_ARENA_H