#include <arena.h>
#include <expander.h>
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
    ExpandedTokenVector* expanded_tokens = expand(pptokens);

    for (size_t i = 0; i < expanded_tokens->count; ++i) {
        printf("%s", expanded_tokens->data[i]->spelling);
    }

    printf("arena usage = %zu KiB\n", arena_usage_KiB());

    return LINUX_EXIT_SUCCESS;
}
