#ifndef SPLICER_H
#define SPLICER_H

#include "normalizer.h"

typedef struct SplicedChar {
    char value;

    SourceChar* source_char;
} SplicedChar;

typedef struct SplicedCharVector {
    SplicedChar** data;

    size_t count;
    size_t capacity;
} SplicedCharVector;

SplicedCharVector* splice(SourceCharVector* source_chars);

#endif  // SPLICER_H
