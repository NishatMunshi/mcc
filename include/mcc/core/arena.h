#ifndef MCC_CORE_ARENA_H
#define MCC_CORE_ARENA_H

#include <stddef.h>

#include "mcc/core/utils.h"

/**
 * @brief Opaque handle for a Memory Arena (Linear Allocator).
 *
 * An Arena allocator manages a pool of memory. It allows for fast allocations
 * by bumping a pointer. All memory allocated is freed at once upon destruction.
 *
 * @note ERROR HANDLING: This module uses "Infallible Allocation".
 * Functions here never return NULL. If the system is out of memory,
 * the program will terminate immediately via mcc_core_error.h.
 */
typedef struct mcc_core_arena mcc_core_arena;

/* --- Constructors & Destructors --- */

/**
 * @brief Creates a new memory arena instance.
 *
 * Allocates the initial internal structures required for the arena.
 *
 * @return A valid pointer to the new arena.
 * @post The program terminates if allocation fails.
 */
mcc_core_arena *mcc_core_arena_construct(size_t capacity);

/**
 * @brief Destroys the arena and frees ALL memory associated with it.
 *
 * This invalidates every pointer ever allocated from this arena.
 *
 * @param self A pointer to the arena to destroy. If NULL, this function does nothing.
 */
void mcc_core_arena_destruct(mcc_core_arena *self);

/* --- Allocators --- */

/**
 * @brief Allocates a block of memory from the arena.
 *
 * This memory is freed automatically when mcc_core_arena_destruct() is called.
 * This memory is invalidated upon calling mcc_core_arena_clear()
 *
 * @param self The arena instance.
 * @param size The size of the memory block requested in bytes.
 * @return A pointer to the allocated memory block, all cleared to 0
 * @warning Never returns NULL. If the allocation fails, the program terminates.
 */
void *mcc_core_arena_allocate(mcc_core_arena *self, size_t size);

/**
 * @brief Helper macro for type-safe array allocation.
 *
 * @param arena The arena instance.
 * @param type The data type to allocate.
 * @param count The number of elements to allocate.
 * @return A casted pointer to the allocated array (Never NULL).
 */
#define MCC_CORE_ARENA_ALLOCATE(arena, type, count) \
    ((type *)mcc_core_arena_allocate((arena), (count * sizeof(type))))

/* --- Modifiers --- */

/**
 * @brief Resets the arena, invalidating all previous allocations.
 *
 * Retains internal memory blocks to allow for rapid re-use.
 *
 * @param self The arena instance to clear.
 */
void mcc_core_arena_clear(mcc_core_arena *self);

#endif  // MCC_CORE_ARENA_H