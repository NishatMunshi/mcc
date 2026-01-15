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

    SplicedChar* splicedchar_start;
    size_t length;
} PPToken;

typedef struct PPTokenVector {
    PPToken* data;

    size_t count;
    size_t capacity;
} PPTokenVector;

PPTokenVector* tokenize(SplicedCharVector* spliced_chars);
void pptoken_print(PPToken pptoken);

#endif  // TOKENIZER_H
