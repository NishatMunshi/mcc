#include <stdio.h>

#include "mcc/core/arena.h"
#include "mcc/core/file.h"
#include "mcc/core/string.h"
#include "mcc/driver/args.h"

int main(int argc, char **argv) {
    mcc_core_arena *permanent_arena = mcc_core_arena_construct();

    mcc_driver_args *command_line_arguments = mcc_driver_args_construct(permanent_arena, argc, argv);
    mcc_core_string *input_file_name = mcc_driver_args_get_input_file_name(command_line_arguments);

    mcc_core_string *source_string = mcc_core_file_read(permanent_arena, input_file_name);
    mcc_core_string_println(source_string, stdout);

    mcc_core_arena_destruct(permanent_arena);
    return 0;
}