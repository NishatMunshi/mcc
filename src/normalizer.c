#include "normalizer.h"

#include "arena.h"
#include "panic.h"
#include "vector.h"

static u8 map(u8 byte) {
    return byte;
}

SourceCharVector* normalize(ByteVector* bytes) {
    SourceCharVector* source_chars = ARENA_ALLOC(SourceCharVector, 1);
    for (size_t i = 0; i < bytes->count; ++i) {
        char c = map(bytes->data[i].value);
        if (c == '\0') panic("NUL byte in file");

        SourceChar source_char = {
            .value = c,
            .byte = bytes->data + i,
        };

        vector_push(source_chars, source_char);
    }

    return source_chars;
}
