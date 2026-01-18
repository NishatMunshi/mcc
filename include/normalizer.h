#ifndef NORMALIZER_H
#define NORMALIZER_H

#include "reader.h"

/*
UTF-32 is chosen as the source character set
as per section 5.2.1 (Character sets) of the
C standard ISO/IEC 9899:2024.
*/
typedef struct SourceChar {
    u32 value;

    ByteVector* origin;
} SourceChar;

typedef struct SourceCharVector {
    SourceChar** data;

    size_t count;
    size_t capacity;
} SourceCharVector;

SourceCharVector* normalize(ByteVector* bytes);

#endif  // NORMALIZER_H
