#include "main.h"

#include "linux.h"
#include "error.h"
#include "file.h"
#include "string.h"
#include "arena.h"

s32 main(s32 argc, char** argv) {
    if(argc < 2) {
        error_fatal("no input file");
    }

    arena_init();

    char* source = file_read(argv[1]);

    size_t source_len = str_len(source);
    linux_write(LINUX_FD_STDOUT, source, source_len);

    return LINUX_EXIT_SUCCESS;
}
