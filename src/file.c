#include "file.h"

#include "arena.h"
#include "diag.h"
#include "error.h"
#include "linux.h"

noreturn void file_error(Token* org_tok, char* msg) {
    if (org_tok != NULL) {
        diag_error(org_tok, msg);
    } else {
        error_fatal(msg);
    }
    while (true);
}

File* file_read(char* filename, Token* org_tok) {
    s32 fd = linux_open(filename, LINUX_FILE_MODE_READONLY, 0);

    // file open failure
    if (fd < 0) {
        file_error(org_tok, "file not found");
    }

    linux_stat_t stat;
    if (linux_fstat(fd, (linux_stat_t*)(&stat)) < 0) {
        linux_close(fd);
        file_error(org_tok, "failed to stat file");
    }

    size_t size = (size_t)stat.st_size;
    u8* buffer = ARENA_ALLOC(u8, size + 1);

    s64 bytes_read = linux_read(fd, buffer, size);
    if (bytes_read < 0) {
        linux_close(fd);
        file_error(org_tok, "failed to read file");
    }

    if (bytes_read != (s64)size) {
        linux_close(fd);
        file_error(org_tok, "partial read of file");
    }

    linux_close(fd);
    buffer[size] = '\0';

    File* file = ARENA_ALLOC(File, 1);

    file->org_tok = org_tok;
    file->name = filename;
    file->og_data = buffer;
    file->og_size = size;

    // assume we have valid data
    // phase1 will make sure
    file->curr_data = (char*)buffer;

    return file;
}
