#include "tokenizer.h"

#include "arena.h"
#include "io.h"
#include "vector.h"
#include "panic.h"

static PPToken pptoken_create(PPTokenKind kind, SplicedChar* splicedchar_start, size_t length) {
    PPToken pptoken = {
        .kind = kind,
        .splicedchar_start = splicedchar_start,
        .length = length,
    };

    return pptoken;
}

static PPToken tokenize_string_literal(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    size_t quote_count = 0;
    while (
        quote_count < 2
    ) {
        if (spliced_chars->data[i].value == '\"') quote_count++;
        if (spliced_chars->data[i].value == '\\') i++, length++;
        if (spliced_chars->data[i].value == '\n') panic("newline inside string literal");
        if (i + 1 == spliced_chars->count) panic("unterminated string literal");
        i++;
        length++;
    }

    return pptoken_create(PP_STRING, spliced_chars->data + start, length);
}

static PPToken tokenize_character_constant(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    size_t quote_count = 0;
    while (
        quote_count < 2
    ) {
        if (spliced_chars->data[i].value == '\'') quote_count++;
        if (spliced_chars->data[i].value == '\\') i++, length++;
        if (spliced_chars->data[i].value == '\n') panic("newline inside string literal");
        if (i + 1 == spliced_chars->count) panic("unterminated string literal");
        i++;
        length++;
    }

    return pptoken_create(PP_CHAR, spliced_chars->data + start, length);
}

static PPToken tokenize_single_line_comment(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        // splicer guarantees file ends in newline
        spliced_chars->data[i++].value != '\n'
    ) {
        length++;
    }
    PPToken pptoken = pptoken_create(PP_WHITESPACE, spliced_chars->data + start, length);
    return pptoken;
}

static PPToken tokenize_block_comment(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        i < spliced_chars->count &&
        !(spliced_chars->data[i].value == '*' &&
          i + 1 < spliced_chars->count &&
          spliced_chars->data[i + 1].value == '/'
        )
    ) {
        length++;
        i++;
    }

    PPToken pptoken = pptoken_create(PP_WHITESPACE, spliced_chars->data + start, length + 2);
    return pptoken;
}

static bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\v' || c == '\f';
}

static PPToken tokenize_whitespace(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        is_space(spliced_chars->data[i].value)
    ) {
        length++;
        i++;
    }

    PPToken pptoken = pptoken_create(PP_WHITESPACE, spliced_chars->data + start, length);
    return pptoken;
}

PPTokenVector* tokenize(SplicedCharVector* spliced_chars) {
    PPTokenVector* pptokens = ARENA_ALLOC(PPTokenVector, 1);
    for (size_t i = 0; i < spliced_chars->count;) {
        // "string literals"
        if ((spliced_chars->data[i].value == 'u' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '8' &&
             i + 2 < spliced_chars->count &&
             spliced_chars->data[i + 2].value == '\"'
            ) ||
            (spliced_chars->data[i].value == 'u' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\"'
            ) ||
            (spliced_chars->data[i].value == 'U' && 
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\"'
            ) ||
            (spliced_chars->data[i].value == 'L' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\"'
            ) || 
            (
                spliced_chars->data[i].value == '\"'
            )
        ) {
            PPToken pptoken = tokenize_string_literal(spliced_chars, i);
            vector_push(pptokens, pptoken);
            i += pptoken.length;
        }

        // 'character constants'
        if ((spliced_chars->data[i].value == 'u' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '8' &&
             i + 2 < spliced_chars->count &&
             spliced_chars->data[i + 2].value == '\''
            ) ||
            (spliced_chars->data[i].value == 'u' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\''
            ) ||
            (spliced_chars->data[i].value == 'U' && 
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\''
            ) ||
            (spliced_chars->data[i].value == 'L' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\''
            ) || 
            (
                spliced_chars->data[i].value == '\''
            )
        ) {
            PPToken pptoken = tokenize_character_constant(spliced_chars, i);
            vector_push(pptokens, pptoken);
            i += pptoken.length;
        }

        // single line comments
        else if (
            spliced_chars->data[i].value == '/' &&
            i + 1 < spliced_chars->count &&
            spliced_chars->data[i + 1].value == '/'
        ) {
            PPToken pptoken = tokenize_single_line_comment(spliced_chars, i);
            vector_push(pptokens, pptoken);
            i += pptoken.length;
        }

        /* BLOCK COMMENTS */
        else if (
            spliced_chars->data[i].value == '/' &&
            i + 1 < spliced_chars->count &&
            spliced_chars->data[i + 1].value == '*'
        ) {
            PPToken pptoken = tokenize_block_comment(spliced_chars, i);
            vector_push(pptokens, pptoken);
            i += pptoken.length;
        }

        // newlines
        else if (
            spliced_chars->data[i].value == '\n'
        ) {
            PPToken pptoken = pptoken_create(PP_NEWLINE, spliced_chars->data + i, 1);
            vector_push(pptokens, pptoken);
            i += 1;
        }

        // whitespace
        else if (
            is_space(spliced_chars->data[i].value)
        ) {
            PPToken pptoken = tokenize_whitespace(spliced_chars, i);
            vector_push(pptokens, pptoken);
            i += pptoken.length;
        }

        else {
            PPToken pptoken = pptoken_create(PP_OTHER, spliced_chars->data + i, 1);
            vector_push(pptokens, pptoken);
            i += 1;
        }
    }

    return pptokens;
}

void pptoken_print(PPToken pptoken) {
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

    puts(ANSI_FG_BRIGHT[pptoken.kind % 8]);
    for (size_t i = 0; i < pptoken.length; ++i) {
        putc(pptoken.splicedchar_start[i].source_char->byte->value);
    }
    puts(ANSI_RESET);
}
