#ifndef IO_H
#define IO_H

#include "linux.h"
#include "types.h"

// Don't use directly, use fprintf macro
// Only understands %c, %s, %zu, %x and %%
void __fprintf_impl(
    s64 rdi,
    s64 rsi,
    s64 rdx,
    s64 rcx,
    s64 r8,
    s64 r9,
    s32 fd,
    char* format,
    ...
);

#define fprintf(fd, format, ...) \
    __fprintf_impl(0, 0, 0, 0, 0, 0, (fd), (format)__VA_OPT__(, ) __VA_ARGS__)

#define printf(format, ...) \
    fprintf(LINUX_FD_STDOUT, (format)__VA_OPT__(, ) __VA_ARGS__)

#define eprintf(format, ...) \
    fprintf(LINUX_FD_STDERR, (format)__VA_OPT__(, ) __VA_ARGS__)

#endif  // IO_H
