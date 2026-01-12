#include "main.h"

#include "linux.h"
#include "error.h"
#include "string.h"
#include "arena.h"

s32 main(s32 argc, char** argv) {
    if(argc < 2) {
        error_fatal("no input file");
    }

    // first thing we do is to init the arena
    arena_init();

    size_t filename_len = str_len(argv[1]);
    linux_write(LINUX_FD_STDOUT, argv[1], filename_len);
    linux_write(LINUX_FD_STDOUT, "\n", 1);

    return LINUX_EXIT_SUCCESS;
}
