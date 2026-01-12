#include "main.h"

#include "arena.h"
#include "error.h"
#include "file.h"
#include "linux.h"

s32 main(s32 argc, char** argv) {
    if (argc < 2) {
        error_fatal("no input file");
    }

    arena_init();

    File* source = file_read(argv[1], NULL);

    (void)source;

    return LINUX_EXIT_SUCCESS;
}
