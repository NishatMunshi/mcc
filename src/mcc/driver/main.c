#include "mcc/driver/main.h"

#include <stdio.h>

#include "mcc/core/arena.h"
#include "mcc/core/string.h"
#include "mcc/core/utils.h"

int main(int argc, char **argv) {
    mcc_core_arena *permanent_arena = mcc_core_arena_construct(MCC_CORE_UTILS_MiB(1));

    if (argc < 1 && argv) {
        return 0;
    }

    mcc_core_string *compare = mcc_core_string_construct_from_c_string(permanent_arena, " \t hello \n   ");

    mcc_core_string_print(mcc_core_string_trim_whitespace(permanent_arena, compare), stdout);

    mcc_core_arena_destruct(permanent_arena);

    return 0;
}