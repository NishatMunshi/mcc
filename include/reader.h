#ifndef READER_H
#define READER_H

#include "types.h"

// forward declaration
typedef struct PPToken PPToken;

// copy of physical file on disk, loaded once
// and pointed to by every include of this file
typedef struct FileDefinition {
    char* full_path;

    u8* content;
    size_t size;
} FileDefinition;

// one #include instance
typedef struct FileInclusion {
    FileDefinition* definition;

    PPToken* pptok;
} FileInclusion;

// smart byte, points to its FileInclusion instance
typedef struct Byte {
    u8 value;

    FileInclusion* inclusion;
    size_t offset;
} Byte;

// containter, not pointers, but objects
typedef struct ByteVector {
    Byte* data;

    size_t count;
    size_t capacity;
} ByteVector;

ByteVector read(char* full_path, PPToken* pptok);

#endif  // READER_H
