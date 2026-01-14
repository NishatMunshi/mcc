#include "main.h"

#include "arena.h"
#include "io.h"
#include "linux.h"
#include "normalizer.h"
#include "panic.h"
#include "reader.h"
#include "splicer.h"

s32 main(s32 argc, char** argv) {
    if (argc < 2) panic("no input file");

    arena_init();

    ByteVector* bytes = read(argv[1], nullptr);

    SourceCharVector* source_chars = normalize(bytes);

    SplicedCharVector* spliced_chars = splice(source_chars);

    for (size_t i = 0; i < spliced_chars->count; ++i) {
        putc(spliced_chars->data[i]->value);
    }

    size_t arena_usage = arena_usage_KiB();
    puts("arena use = ");
    putu(arena_usage);
    puts(" KiB\n");
    return LINUX_EXIT_SUCCESS;
}
