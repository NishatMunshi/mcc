#include "mcc/driver/main.h"

#include <stdio.h>

#include "mcc/core/arena.h"
#include "mcc/core/utils.h"

int main(int argc, char **argv) {
    mcc_core_arena *permanent_arena = mcc_core_arena_construct(MCC_CORE_UTILS_MiB(1));

    void *ptr1 = mcc_core_arena_allocate(permanent_arena, 20);
    void *ptr2 = mcc_core_arena_allocate(permanent_arena, MCC_CORE_UTILS_MiB(1));

    printf("%d, %p\n", argc, (void *)argv);
    printf("%p, %p\n", ptr1, ptr2);

    mcc_core_arena_destruct(permanent_arena);

    return 0;
}