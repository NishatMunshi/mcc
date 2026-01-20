#include <arena.h>
#include <tokenizer.h>
#include <io.h>
#include <linux.h>
#include <main.h>
#include <panic.h>

s32 main(s32 argc, char** argv) {
    if (argc < 2) panic("no input file");

    arena_init();

    ByteVector* bytes = read(argv[1], nullptr);
    SourceCharVector* source_chars = normalize(bytes);
    SplicedCharVector* spliced_chars = splice(source_chars);
    PPTokenVector* pptokens = tokenize(spliced_chars);

    printf("|");
    for(size_t i = 0; i< pptokens->count; ++i) {
        printf("%s", pptokens->data[i]->spelling);
        printf("|");
    }

    printf("arena usage = %zu KiB\n", arena_usage_KiB());

    return LINUX_EXIT_SUCCESS;
}
