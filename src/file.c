#include "linux.h"
#include "arena.h"
#include "types.h"
#include "error.h"

char* file_read(char* filename) {
    s32 fd = linux_open(filename, LINUX_FILE_MODE_READONLY, 0);
    if (fd < 0) {
        error_fatal("could not open input file");
    }

    linux_stat_t stat;
    if (linux_fstat(fd, (linux_stat_t*)(&stat)) < 0) {
        linux_close(fd);
        error_fatal("could not stat input file");
    }

    size_t size = (size_t)stat.st_size;
    char *buffer = (char*)arena_alloc(size + 1);

    s64 bytes_read = linux_read(fd, buffer, size);    
    if (bytes_read < 0) {
        linux_close(fd);
        error_fatal("failed to read input file");
    }
    
    if (bytes_read != (s64)size) {
        linux_close(fd);
        error_fatal("partial read of input file");
    }

    linux_close(fd);
    buffer[size] = '\0';

    return buffer;
}
