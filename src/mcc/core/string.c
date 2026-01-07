#include "mcc/core/string.h"

#include <ctype.h>
#include <string.h>

#include "mcc/core/arena.h"
#include "mcc/core/error.h"

struct mcc_core_string {
    size_t len;
    char* data;
};

mcc_core_string* mcc_core_string_construct(mcc_core_arena* arena, char* start, size_t len) {
    // debug safety
    if (arena == NULL) mcc_core_error_fatal("mcc_core_string_construct_from_range: arena is NULL");
    // Only crash if user asks to copy data from NULL
    if (len > 0 && start == NULL) mcc_core_error_fatal("mcc_core_string_construct_from_range: start is NULL with non-zero len");

    mcc_core_string* self = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_string, 1);
    self->data = MCC_CORE_ARENA_ALLOCATE(arena, char, len + 1);

    if (len > 0) {
        memcpy(self->data, start, len);
    }

    self->data[len] = '\0';
    self->len = len;
    return self;
}

mcc_core_string* mcc_core_string_construct_from_cstr(mcc_core_arena* arena, char* cstr) {
    // debug safety
    if (arena == NULL) mcc_core_error_fatal("mcc_core_string_construct_from_cstr: arena is NULL");
    if (cstr == NULL) mcc_core_error_fatal("mcc_core_string_construct_from_cstr: cstr is NULL");

    return mcc_core_string_construct(arena, cstr, strlen(cstr));
}

// comparators
bool mcc_core_string_equals(mcc_core_string* self, mcc_core_string* other) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_equals_string: self is NULL");
    if (other == NULL) mcc_core_error_fatal("mcc_core_string_equals_string: other is NULL");

    if (self->len != other->len) {
        return false;
    }

    if (self->len == 0) {
        return true;
    }

    return !memcmp(self->data, other->data, self->len);
}

bool mcc_core_string_equals_cstr(mcc_core_string* self, char* cstr) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_equals_cstr: self is NULL");
    if (cstr == NULL) mcc_core_error_fatal("mcc_core_string_equals_cstr: cstr is NULL");

    mcc_core_string mcc_cstr = {.data = cstr, .len = strlen(cstr)};

    return mcc_core_string_equals(self, &mcc_cstr);
}

bool mcc_core_string_starts_with(mcc_core_string* self, mcc_core_string* prefix) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_starts_with_string: self is NULL");
    if (prefix == NULL) mcc_core_error_fatal("mcc_core_string_starts_with_string: prefix is NULL");

    if (self->len < prefix->len) {
        return false;
    }

    return memcmp(self->data, prefix->data, prefix->len) == 0;
}

bool mcc_core_string_starts_with_cstr(mcc_core_string* self, char* prefix) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_starts_with_cstr: self is NULL");
    if (prefix == NULL) mcc_core_error_fatal("mcc_core_string_starts_with_cstr: prefix is NULL");

    mcc_core_string mcc_prefix = {.data = prefix, .len = strlen(prefix)};

    return mcc_core_string_starts_with(self, &mcc_prefix);
}

// hashing - FNV-1a
uint64_t mcc_core_string_hash(mcc_core_string* self) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_hash: self is NULL");

    uint64_t hash = 0xcbf29ce484222325;

    for (size_t i = 0; i < self->len; ++i) {
        hash = hash ^ (uint8_t)(self->data[i]);
        hash = hash * 0x100000001b3;
    }

    return hash;
}

// utilities
mcc_core_string* mcc_core_string_substring(mcc_core_arena* arena, mcc_core_string* self, size_t begin, size_t len) {
    // debug safety
    if (arena == NULL) mcc_core_error_fatal("mcc_core_string_substring: arena is NULL");
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_substring: self is NULL");
    // begin == 0 is perfectly valid
    // len == 0 is perfectly valid (empty string is a substring of all valid strings)

    // 1. Check if 'begin' itself is out of bounds
    if (begin > self->len) {
        mcc_core_error_fatal("mcc_core_string_substring: 'begin' index out of bounds");
    }

    // 2. Check if 'len' goes past the end (Safe subtraction prevents overflow)
    if (len > self->len - begin) {
        mcc_core_error_fatal("mcc_core_string_substring: len exceeds string bounds");
    }

    // 3. Delegate to the range constructor (This correctly copies/Owns the memory)
    // len == 0 is handled inside the constructor safely
    char* start = self->data + begin;
    return mcc_core_string_construct(arena, start, len);
}

mcc_core_string* mcc_core_string_trim_whitespace(mcc_core_arena* arena, mcc_core_string* self) {
    // debug safety
    if (arena == NULL) mcc_core_error_fatal("mcc_core_string_trim_whitespace: arena is NULL");
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_trim_whitespace: self is NULL");

    // We don't need to check for empty explicitly.
    // If self is empty, len starts at 0, loops skip, and we make a new empty string.

    size_t begin = 0;
    size_t len = self->len;

    // trim beginning
    while (len > 0 && isspace((unsigned char)self->data[begin])) {
        begin++;
        len--;
    }

    // trim end
    while (len > 0 && isspace((unsigned char)self->data[begin + len - 1])) {
        len--;
    }

    // Always returns a new string on the provided 'arena'
    return mcc_core_string_construct(arena, self->data + begin, len);
}

void mcc_core_string_print(mcc_core_string* self, FILE* stream) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_print: self is NULL");
    if (stream == NULL) mcc_core_error_fatal("mcc_core_string_print: stream is NULL");

    fwrite(self->data, 1, self->len, stream);
}

void mcc_core_string_println(mcc_core_string* self, FILE* stream) {
    // debug safety
    if (self == NULL) mcc_core_error_fatal("mcc_core_string_print: self is NULL");
    if (stream == NULL) mcc_core_error_fatal("mcc_core_string_print: stream is NULL");

    fwrite(self->data, 1, self->len, stream);
    fputc('\n', stream);
}