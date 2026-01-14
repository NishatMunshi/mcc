#include "normalizer.h"

#include "arena.h"
#include "panic.h"
#include "vector.h"

char map(u8 byte) {
    return (char)byte;
}

SourceCharVector* normalize(ByteVector* bytes) {
    SourceCharVector* source_chars = ARENA_ALLOC(SourceCharVector, 1);
    for (size_t i = 0; i < bytes->count; ++i) {
        SourceChar* source_char = ARENA_ALLOC(SourceChar, 1);

        char c = map(bytes->data[i]->value);
        if (c == '\0') panic("NUL byte in file");
        source_char->value = c;
        source_char->byte = bytes->data[i];

        vector_push(source_chars, source_char);
    }

    return source_chars;
}
