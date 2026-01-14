#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "splicer.h"

typedef enum PPTokenKind {
    PP_EOF = 0,
    PP_HEADER_NAME,  // <stdio.h>
    PP_IDENTIFIER,   // main
    PP_NUMBER,       // 123
    PP_CHAR,         // 'a'
    PP_STRING,       // "text"
    PP_PUNCTUATOR,   // +
    PP_OTHER         // @
} PPTokenKind;

typedef struct PPToken {
    PPTokenKind kind;

    SplicedChar* sourcechar_start;
    size_t len;
} PPToken;

typedef struct PPTokenVector {
    PPToken** data;

    size_t count;
    size_t capacity;
} PPTokenVector;

PPTokenVector* tokenize(SplicedCharVector* spliced_chars);

#endif  // TOKENIZER_H
