#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "splicer.h"

typedef enum PPTokenKind {
    PP_WHITESPACE = 0,
    PP_PUNCTUATOR,  // +
    PP_STRING,      // "text"
    PP_IDENTIFIER,  // main
    PP_NUMBER,      // 123
    PP_CHAR,        // 'a'
    PP_HEADERNAME,  // <stdio.h>
    PP_OTHER,       // @
    PP_NEWLINE
} PPTokenKind;

typedef struct PPToken {
    PPTokenKind kind;
    char* spelling;
    size_t length;

    SplicedChar* splicedchar_start;
} PPToken;

typedef struct PPTokenVector {
    PPToken* data;

    size_t count;
    size_t capacity;
} PPTokenVector;

PPTokenVector tokenize(SplicedCharVector spliced_chars);
bool pptoken_is(PPToken pptoken, PPTokenKind kind, char* spelling);

#endif  // TOKENIZER_H
