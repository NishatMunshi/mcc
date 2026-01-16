#ifndef IO_H
#define IO_H

#include "types.h"

// Don't use directly, use printf macro
void __printf_impl(
    s64 rdi,
    s64 rsi,
    s64 rdx,
    s64 rcx,
    s64 r8,
    s64 r9,
    char* format,
    ...
);

// only understands %c, %s, %zu, %x and %%
#define printf(format, ...) \
    __printf_impl(0, 0, 0, 0, 0, 0, (format)__VA_OPT__(, ) __VA_ARGS__)

#endif  // IO_H
