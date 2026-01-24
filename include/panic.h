#ifndef PANIC_H
#define PANIC_H

#include "io.h"
#include "linux.h"

#define panic(format, ...)                           \
    do {                                             \
        eprintf("mcc: error: ");                     \
        eprintf((format)__VA_OPT__(, ) __VA_ARGS__); \
        eprintf("\n");                               \
        linux_exit(LINUX_EXIT_FAILURE);              \
    } while (0)

#endif  // PANIC_H
