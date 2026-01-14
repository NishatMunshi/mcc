#include "tokenizer.h"

#include "arena.h"
#include "io.h"
#include "vector.h"

static PPToken pptoken_create(PPTokenKind kind, SplicedChar* splicedchar_start, size_t length) {
    PPToken pptoken = {
        .kind = kind,
        .splicedchar_start = splicedchar_start,
        .length = length,
    };

    return pptoken;
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
    while(
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
        // single line comments
        if (
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

static void pptoken_print_kind(PPToken pptoken) {
    switch (pptoken.kind) {
        case PP_WHITESPACE: puts("<PP_WHITESPACE>"); return;
        case PP_NEWLINE:    puts("<PP_NEWLINE>"); return;
        case PP_OTHER:      puts("<PP_OTHER>"); return;
        default:            puts("<PP_UNKNOWN>"); return;
    }
}

void pptoken_print(PPToken pptoken) {
    pptoken_print_kind(pptoken);

    puts(": ||");
    for (size_t i = 0; i < pptoken.length; ++i) {
        putc(pptoken.splicedchar_start[i].value);
    }
    puts("||\n");
}
