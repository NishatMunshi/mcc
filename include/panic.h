#ifndef PANIC_H
#define PANIC_H

#include "io.h"
#include "linux.h"
#include "reader.h"

#define panic(format, ...)                           \
    do {                                             \
        eprintf("mcc: error: ");                     \
        eprintf((format)__VA_OPT__(, ) __VA_ARGS__); \
        eprintf("\n");                               \
                                                     \
        linux_exit(LINUX_EXIT_FAILURE);              \
    } while (0)

#define print_header(loc, format, ...)                                  \
    do {                                                                \
        eprintf("%s:%zu:%zu: ", (loc).filename, (loc).line, (loc).col); \
        eprintf((format)__VA_OPT__(, ) __VA_ARGS__);                    \
        eprintf("\n");                                                  \
    } while (0)

typedef struct Location {
    char* filename;
    size_t line;
    size_t col;
} Location;

Location byte_get_location(Byte* byte);
void print_snippet(FileDefinition* def, size_t line);
void print_squiggles(size_t line, size_t col);

#define panic_byte(byte, format, ...)                          \
    do {                                                       \
        Location loc = byte_get_location((byte));              \
                                                               \
        print_header(loc, (format)__VA_OPT__(, ) __VA_ARGS__); \
        print_snippet((byte)->origin->definition, loc.line);   \
        print_squiggles(loc.line, loc.col);                    \
                                                               \
        linux_exit(LINUX_EXIT_FAILURE);                        \
    } while (0)

#endif  // PANIC_H
