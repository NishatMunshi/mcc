#include "main.h"

#include "arena.h"
#include "error.h"
#include "file.h"
#include "iostream.h"
#include "linux.h"
#include "normalizer.h"
#include "splicer.h"

s32 main(s32 argc, char** argv) {
    if (argc < 2) {
        error_fatal("no input file");
    }

    arena_init();

    File* source_file = file_read(argv[1], NULL);

    normalize(source_file);

    splice(source_file);

    // iostream_print_str(IOSTREAM_STDOUT, source_file->curr_data);

    iostream_print_str(IOSTREAM_STDOUT, "arena memory used = ");
    iostream_print_uint(IOSTREAM_STDOUT, arena_usage_KiB());
    iostream_print_str(IOSTREAM_STDOUT, " KiB \n");
    return LINUX_EXIT_SUCCESS;
}
