#ifndef NORMALIZER_H
#define NORMALIZER_H

#include "reader.h"

typedef struct SourceChar {
    char value;

    Byte* byte;
} SourceChar;

typedef struct SourceCharVector {
    SourceChar** data;

    size_t count;
    size_t capacity;
} SourceCharVector;

SourceCharVector* normalize(ByteVector* bytes);

#endif  // NORMALIZER_H
