#include <arena.h>
#include <linux.h>
#include <panic.h>
#include <utils.h>

#define ARENA_KiB(x) ((x) << 10)
#define ARENA_MiB(x) ((x) << 20)
#define ARENA_SIZE_DEFAULT (ARENA_MiB(1))
#define ARENA_ALIGNMENT_BYTES (alignof(max_align_t))
#define ARENA_ALIGN_UP(number) \
    (((number) + ARENA_ALIGNMENT_BYTES - 1) & ~(ARENA_ALIGNMENT_BYTES - 1))

typedef struct Arena {
    size_t capacity;
    size_t position;
    u8* heap;
} Arena;

static Arena arena = {
    .heap = nullptr,
    .capacity = 0,
    .position = 0,
};

static u8* arena_ask_os_for_mem(size_t request) {
    // get current break
    u8* curr_brk = linux_brk(LINUX_BRK_CURRENT);

    size_t req_aligned = ARENA_ALIGN_UP(request);
    u8* next_brk_req = curr_brk + req_aligned;

    // ask for memory
    u8* next_brk = linux_brk(next_brk_req);

    // handle OS refusal
    if (next_brk != next_brk_req) panic("out of memory");

    return curr_brk;
}

static void arena_grow(size_t needed_size) {
    size_t request = UTILS_MAX(needed_size, ARENA_SIZE_DEFAULT);
    arena_ask_os_for_mem(request);
    arena.capacity += request;
}

void arena_init(void) {
    u8* heap_start = arena_ask_os_for_mem(ARENA_SIZE_DEFAULT);

    arena.capacity = ARENA_SIZE_DEFAULT;
    arena.position = 0;
    arena.heap = heap_start;
}

void* arena_alloc(size_t size) {
    size_t aligned_size = ARENA_ALIGN_UP(size);

    if (arena.position + aligned_size > arena.capacity) {
        arena_grow(aligned_size);
    }

    u8* pointer = arena.heap + arena.position;
    arena.position += aligned_size;

    return (void*)pointer;
}

size_t arena_usage_KiB(void) {
    return arena.position / ARENA_KiB(1);
}
