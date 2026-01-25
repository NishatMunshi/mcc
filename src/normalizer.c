#include <arena.h>
#include <normalizer.h>
#include <panic.h>
#include <vector.h>

typedef struct ByteStream {
    ByteVector* bytes;
    size_t current_index;
} ByteStream;

/*
Peeks ahead by offset bytes without crashing.
Upon reaching EOF, returns a valid pointer to
a dummy NUL Byte which lives on the static memory.
*/
static Byte* stream_peekahead(ByteStream* stream, size_t offset) {
    static Byte EOF_SENTINEL = {
        .value = 0,
        .origin = nullptr,
        .offset = 0,
    };

    size_t target_index = stream->current_index + offset;

    if (target_index >= stream->bytes->count) {
        return &EOF_SENTINEL;
    }

    return stream->bytes->data[target_index];
}

/*
Consumes a maximum of count number of bytes.
Updates the stream index automatically.
If count number of bytes are not available,
consumes everything that IS available.
*/
static void stream_consume(ByteStream* stream, size_t count) {
    if (stream->current_index + count >= stream->bytes->count) {
        stream->current_index = stream->bytes->count;
    }

    else {
        stream->current_index += count;
    }
}

static SourceChar* normalize_1byte_sequence(ByteStream* stream) {
    Byte* byte0 = stream_peekahead(stream, 0);
    u8 b0 = byte0->value;

    SourceChar* source_char = ARENA_ALLOC(SourceChar, 1);
    source_char->value = (b0 & 0x7f);
    source_char->origin = ARENA_ALLOC(ByteVector, 1);
    vector_push(source_char->origin, byte0);

    stream_consume(stream, 1);

    return source_char;
}

static SourceChar* normalize_2byte_sequence(ByteStream* stream) {
    Byte* byte0 = stream_peekahead(stream, 0);
    Byte* byte1 = stream_peekahead(stream, 1);
    u8 b0 = byte0->value;
    u8 b1 = byte1->value;

    if ((b1 & 0xc0) != 0x80) {
        panic_byte(byte0, "invalid 2-byte UTF-8 sequence");
    }

    SourceChar* source_char = ARENA_ALLOC(SourceChar, 1);
    source_char->value = (((b0 & 0x1f) << 6) | (b1 & 0x3f));
    source_char->origin = ARENA_ALLOC(ByteVector, 1);
    vector_push(source_char->origin, byte0);
    vector_push(source_char->origin, byte1);

    stream_consume(stream, 2);

    return source_char;
}

static SourceChar* normalize_3byte_sequence(ByteStream* stream) {
    Byte* byte0 = stream_peekahead(stream, 0);
    Byte* byte1 = stream_peekahead(stream, 1);
    Byte* byte2 = stream_peekahead(stream, 2);
    u8 b0 = byte0->value;
    u8 b1 = byte1->value;
    u8 b2 = byte2->value;

    if ((b1 & 0xc0) != 0x80 || (b2 & 0xc0) != 0x80) {
        panic_byte(byte0, "invalid 3-byte UTF-8 sequence");
    }

    SourceChar* source_char = ARENA_ALLOC(SourceChar, 1);
    source_char->value = (((b0 & 0x0f) << 12) | ((b1 & 0x3f) << 6) | (b2 & 0x3f));
    source_char->origin = ARENA_ALLOC(ByteVector, 1);
    vector_push(source_char->origin, byte0);
    vector_push(source_char->origin, byte1);
    vector_push(source_char->origin, byte2);

    stream_consume(stream, 3);

    return source_char;
}

static SourceChar* normalize_4byte_sequence(ByteStream* stream) {
    Byte* byte0 = stream_peekahead(stream, 0);
    Byte* byte1 = stream_peekahead(stream, 1);
    Byte* byte2 = stream_peekahead(stream, 2);
    Byte* byte3 = stream_peekahead(stream, 3);
    u8 b0 = byte0->value;
    u8 b1 = byte1->value;
    u8 b2 = byte2->value;
    u8 b3 = byte3->value;

    if ((b1 & 0xc0) != 0x80 || (b2 & 0xc0) != 0x80 || (b3 & 0xc0) != 0x80) {
        panic_byte(byte0, "invalid 4-byte UTF-8 sequence");
    }

    SourceChar* source_char = ARENA_ALLOC(SourceChar, 1);
    source_char->value = (((b0 & 0x07) << 18) | ((b1 & 0x3f) << 12) | ((b2 & 0x3f) << 6) | (b3 & 0x3f));
    source_char->origin = ARENA_ALLOC(ByteVector, 1);
    vector_push(source_char->origin, byte0);
    vector_push(source_char->origin, byte1);
    vector_push(source_char->origin, byte2);
    vector_push(source_char->origin, byte3);

    stream_consume(stream, 4);

    return source_char;
}

SourceCharVector* normalize(ByteVector* bytes) {
    ByteStream stream = {
        .bytes = bytes,
        .current_index = 0,
    };

    SourceCharVector* source_chars = ARENA_ALLOC(SourceCharVector, 1);

    for (Byte* byte = stream_peekahead(&stream, 0);
         byte->value != 0;
         byte = stream_peekahead(&stream, 0)) {
        u8 byte_val = byte->value;
        SourceChar* source_char = nullptr;

        if ((byte_val & 0x80) == 0x00) {
            source_char = normalize_1byte_sequence(&stream);
        }

        else if ((byte_val & 0xe0) == 0xc0) {
            source_char = normalize_2byte_sequence(&stream);
        }

        else if ((byte_val & 0xf0) == 0xe0) {
            source_char = normalize_3byte_sequence(&stream);
        }

        else if ((byte_val & 0xf8) == 0xf0) {
            source_char = normalize_4byte_sequence(&stream);
        }

        else {
            panic_byte(byte, "invalid UTF-8 detected");
        }

        vector_push(source_chars, source_char);
    }

    return source_chars;
}
