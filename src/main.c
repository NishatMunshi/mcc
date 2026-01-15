#include "main.h"

#include "arena.h"
#include "io.h"
#include "linux.h"
#include "normalizer.h"
#include "panic.h"
#include "reader.h"
#include "splicer.h"
#include "tokenizer.h"

s32 main(s32 argc, char** argv) {
    if (argc < 2) panic("no input file");

    arena_init();

    ByteVector* bytes = read(argv[1], nullptr);
    SourceCharVector* source_chars = normalize(bytes);
    SplicedCharVector* spliced_chars = splice(source_chars);
    PPTokenVector* pptokens = tokenize(spliced_chars);

    for (size_t i = 0; i < pptokens->count; ++i) {
        // colors the tokens according to type
        pptoken_print(pptokens->data[i]);
    }

    size_t arena_usage = arena_usage_KiB();
    puts("arena use = ");
    putu(arena_usage);
    puts(" KiB\n");
    return LINUX_EXIT_SUCCESS;
}
