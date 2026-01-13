#include "arena.h"

#include "error.h"
#include "linux.h"
#include "stdalign.h"
#include "utils.h"

#define ARENA_KiB(x) ((x) << 10)
#define ARENA_MiB(x) ((x) << 20)
#define ARENA_SIZE_DEFAULT (ARENA_MiB(1))
#define ARENA_ALIGNMENT_BYTES (alignof(max_align_t))
#define ARENA_ALIGN_UP(number) \
    (((number) + ARENA_ALIGNMENT_BYTES - 1) & ~(ARENA_ALIGNMENT_BYTES - 1))

typedef struct Arena {
    size_t cap;
    size_t pos;
    u8* heap;
} Arena;

// the only global variable
static Arena arena;

static u8* arena_ask_os_for_mem(size_t request) {
    // get current break
    u8* curr_brk = linux_brk(LINUX_BRK_CURRENT);

    size_t req_aligned = ARENA_ALIGN_UP(request);
    u8* next_brk_req = curr_brk + req_aligned;

    // ask for memory
    u8* next_brk = linux_brk(next_brk_req);

    // handle OS refusal
    if (next_brk != next_brk_req) {
        error_fatal("out of memory");
    }

    return curr_brk;
}

void arena_init(void) {
    u8* heap_start = arena_ask_os_for_mem(ARENA_SIZE_DEFAULT);

    arena.cap = ARENA_SIZE_DEFAULT;
    arena.pos = 0;
    arena.heap = heap_start;
}

static void arena_grow(size_t needed_size) {
    size_t req = UTILS_MAX(needed_size, ARENA_SIZE_DEFAULT);
    arena_ask_os_for_mem(req);
    arena.cap += req;
}

void* arena_alloc(size_t size) {
    size_t aligned_size = ARENA_ALIGN_UP(size);

    if (arena.pos + aligned_size > arena.cap) {
        arena_grow(aligned_size);
    }

    u8* ptr = arena.heap + arena.pos;
    arena.pos += aligned_size;

    return (void*)ptr;
}

size_t arena_usage_KiB(void) {
    return arena.pos / ARENA_KiB(1);
}
