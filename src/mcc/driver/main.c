#include <stdio.h>

#include "mcc/core/arena.h"
#include "mcc/core/string.h"
#include "mcc/driver/args.h"

int main(int argc, char **argv) {
    mcc_core_arena *arena_permanent = mcc_core_arena_construct();

    mcc_driver_args *command_line_arguments = mcc_driver_args_construct(arena_permanent, argc, argv);

    mcc_core_string *input_file_name = mcc_driver_args_get_input_file_name(command_line_arguments);
    mcc_core_string_println(input_file_name, stdout);

    mcc_core_arena_destruct(arena_permanent);
    return 0;
}