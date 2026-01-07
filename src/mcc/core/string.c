#include "mcc/core/string.h"

#include <ctype.h>
#include <string.h>

#include "mcc/core/arena.h"
#include "mcc/core/error.h"

struct mcc_core_string {
    size_t length;
    char* data;
};

// constructors
mcc_core_string* mcc_core_string_construct_from_c_string(mcc_core_arena* arena, char* c_string) {
    // debug safety
    if (arena == NULL) mcc_core_error_fatal("mcc_core_string_construct_from_c_string: arena is NULL");
    if (c_string == NULL) mcc_core_error_fatal("mcc_core_string_construct_from_c_string: c_string is NULL");

    mcc_core_string* string = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_string, 1);

    size_t length = strlen(c_string);

    // we allocate 1 extra byte to respect the c style null terminator
    string->data = MCC_CORE_ARENA_ALLOCATE(arena, char, length + 1);
    memcpy(string->data, c_string, length + 1);

    // Always null-terminate
    string->data[length] = '\0';
    string->length = length;

    return string;
}

mcc_core_string* mcc_core_string_construct_from_range(mcc_core_arena* arena, char* start, size_t length) {
    // debug safety
    if (arena == NULL) mcc_core_error_fatal("mcc_core_string_construct_from_range: arena is NULL");
    // Only crash if user asks to copy data from NULL
    if (length > 0 && start == NULL) mcc_core_error_fatal("mcc_core_string_construct_from_range: start is NULL with non-zero length");

    mcc_core_string* string = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_string, 1);
    string->data = MCC_CORE_ARENA_ALLOCATE(arena, char, length + 1);

    if (length > 0) {
        memcpy(string->data, start, length);
    }

    string->data[length] = '\0';
    string->length = length;
    return string;
}

// comparators
bool mcc_core_string_equals_string(const mcc_core_string* self, const mcc_core_string* other) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_equals_string: self is NULL");
    if (other == NULL) mcc_core_error_fatal("mcc_core_string_equals_string: other is NULL");

    if (self->length != other->length) {
        return false;
    }

    if (self->length == 0) {
        return true;
    }

    return !memcmp(self->data, other->data, self->length);
}

bool mcc_core_string_equals_c_string(const mcc_core_string* self, const char* c_string) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_equals_c_string: self is NULL");
    if (c_string == NULL) mcc_core_error_fatal("mcc_core_string_equals_c_string: c_string is NULL");

    size_t c_string_length = strlen(c_string);

    if (self->length != c_string_length) {
        return false;
    }

    if (self->length == 0) {
        return true;
    }

    return !memcmp(self->data, c_string, self->length);
}

bool mcc_core_string_starts_with(const mcc_core_string* self, const mcc_core_string* prefix) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_starts_with: self is NULL");
    if (prefix == NULL) mcc_core_error_fatal("mcc_core_string_starts_with: prefix is NULL");

    if (self->length < prefix->length) {
        return false;
    }

    return memcmp(self->data, prefix->data, prefix->length) == 0;
}

// hashing - FNV-1a
uint64_t mcc_core_string_hash(const mcc_core_string* self) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_hash: self is NULL");

    uint64_t hash = 0xcbf29ce484222325;

    for (size_t i = 0; i < self->length; ++i) {
        hash = hash ^ (uint8_t)(self->data[i]);
        hash = hash * 0x100000001b3;
    }

    return hash;
}

// utilities
mcc_core_string* mcc_core_string_substring(mcc_core_arena* arena, const mcc_core_string* self, size_t begin, size_t length) {
    // debug safety
    if (arena == NULL) mcc_core_error_fatal("mcc_core_string_substring: arena is NULL");
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_substring: self is NULL");
    // begin == 0 is perfectly valid
    // length == 0 is perfectly valid (empty string is a substring of all valid strings)

    // 1. Check if 'begin' itself is out of bounds
    if (begin > self->length) {
        mcc_core_error_fatal("mcc_core_string_substring: 'begin' index out of bounds");
    }

    // 2. Check if 'length' goes past the end (Safe subtraction prevents overflow)
    if (length > self->length - begin) {
        mcc_core_error_fatal("mcc_core_string_substring: length exceeds string bounds");
    }

    // 3. Delegate to the range constructor (This correctly copies/Owns the memory)
    // length == 0 is handled inside the constructor safely
    char* start = self->data + begin;
    return mcc_core_string_construct_from_range(arena, start, length);
}

mcc_core_string* mcc_core_string_trim_whitespace(mcc_core_arena* arena, const mcc_core_string* self) {
    // debug safety
    if (arena == NULL) mcc_core_error_fatal("mcc_core_string_trim_whitespace: arena is NULL");
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_trim_whitespace: self is NULL");

    // We don't need to check for empty explicitly.
    // If self is empty, length starts at 0, loops skip, and we make a new empty string.

    size_t begin = 0;
    size_t length = self->length;

    // trim beginning
    while (length > 0 && isspace((unsigned char)self->data[begin])) {
        begin++;
        length--;
    }

    // trim end
    while (length > 0 && isspace((unsigned char)self->data[begin + length - 1])) {
        length--;
    }

    // Always returns a new string on the provided 'arena'
    return mcc_core_string_construct_from_range(arena, self->data + begin, length);
}

void mcc_core_string_print(const mcc_core_string* self, FILE* output_stream) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_print: self is NULL");
    if (output_stream == NULL) mcc_core_error_fatal("mcc_core_string_print: output_stream is NULL");

    fwrite(self->data, 1, self->length, output_stream);
}

void mcc_core_string_println(const mcc_core_string* self, FILE* output_stream) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_print: self is NULL");
    if (output_stream == NULL) mcc_core_error_fatal("mcc_core_string_print: output_stream is NULL");

    fwrite(self->data, 1, self->length, output_stream);
    fputc('\n', output_stream);
}