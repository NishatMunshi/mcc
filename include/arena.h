#ifndef ARENA_H
#define ARENA_H

#include "types.h"

void arena_init(void);
void* arena_alloc(size_t size);

#define ARENA_ALLOC(Type, count) \
    ((Type*)arena_alloc((count) * sizeof(Type)))

#endif  // ARENA_H
