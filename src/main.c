#include <arena.h>
#include <expander.h>
#include <io.h>
#include <linux.h>
#include <main.h>
#include <panic.h>

void expanded_token_print(ExpandedToken expanded_token) {
    char* ANSI_RESET = "\x1b[0m";
    char* ANSI_FG_BRIGHT[] = {
        "\x1b[90m",  // 0: Bright Black (Gray)
        "\x1b[91m",  // 1: Bright Red
        "\x1b[92m",  // 2: Bright Green
        "\x1b[93m",  // 3: Bright Yellow
        "\x1b[94m",  // 4: Bright Blue
        "\x1b[95m",  // 5: Bright Magenta
        "\x1b[96m",  // 6: Bright Cyan
        "\x1b[97m",  // 7: Bright White
    };

    if(expanded_token.invocation)
    printf("%s", ANSI_FG_BRIGHT[expanded_token.kind % 8]);
    printf("%s", expanded_token.spelling);
    if(expanded_token.invocation)
    printf("%s", ANSI_RESET);
}

s32 main(s32 argc, char** argv) {
    if (argc < 2) panic("no input file");

    arena_init();

    ByteVector bytes = read(argv[1], nullptr);
    SourceCharVector source_chars = normalize(bytes);
    SplicedCharVector spliced_chars = splice(source_chars);
    PPTokenVector pptokens = tokenize(spliced_chars);
    ExpandedTokenVector expanded_tokens = expand(pptokens);

    for (size_t i = 0; i < expanded_tokens.count; ++i) {
        // colors the tokens according to type
        expanded_token_print(expanded_tokens.data[i]);
    }

    printf("arena usage = %zu KiB\n", arena_usage_KiB());
    return LINUX_EXIT_SUCCESS;
}
