#include "mcc/core/string.h"

#include <ctype.h>
#include <string.h>

#include "mcc/core/arena.h"
#include "mcc/core/error.h"

struct mcc_core_string {
    size_t len;
    char* data;
};

mcc_core_string* mcc_core_string_construct_copy(mcc_core_arena* arena, char* start, size_t len) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(arena);
    // Only crash if user asks to copy data from NULL
    if(len > 0) MCC_CORE_ERROR_CHECK_NULL(start);

    mcc_core_string* self = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_string, 1);
    self->data = MCC_CORE_ARENA_ALLOCATE(arena, char, len + 1);

    if (len > 0) {
        memcpy(self->data, start, len);
    }

    self->data[len] = '\0';
    self->len = len;
    return self;
}

mcc_core_string* mcc_core_string_construct_copy_from_cstr(mcc_core_arena* arena, char* cstr) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(arena);
    MCC_CORE_ERROR_CHECK_NULL(cstr);

    return mcc_core_string_construct_copy(arena, cstr, strlen(cstr));
}

mcc_core_string* mcc_core_string_construct_move_from_cstr(mcc_core_arena* arena, char* cstr, size_t len) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(arena);
    if (len > 0) MCC_CORE_ERROR_CHECK_NULL(cstr);

    mcc_core_string* self = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_string, 1);

    // just take ownership, forget about the buffer
    self->data = cstr;
    self->len = len;

    // null terminate for consistensy with the other constructors
    self->data[len] = '\0';
    return self;
}

// comparators
bool mcc_core_string_equals(mcc_core_string* self, mcc_core_string* other) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(self);
    MCC_CORE_ERROR_CHECK_NULL(other);

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
    MCC_CORE_ERROR_CHECK_NULL(self);
    MCC_CORE_ERROR_CHECK_NULL(cstr);

    mcc_core_string mcc_cstr = {.data = cstr, .len = strlen(cstr)};

    return mcc_core_string_equals(self, &mcc_cstr);
}

bool mcc_core_string_starts_with(mcc_core_string* self, mcc_core_string* prefix) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(self);
    MCC_CORE_ERROR_CHECK_NULL(prefix);

    if (self->len < prefix->len) {
        return false;
    }

    return memcmp(self->data, prefix->data, prefix->len) == 0;
}

bool mcc_core_string_starts_with_cstr(mcc_core_string* self, char* prefix) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(self);
    MCC_CORE_ERROR_CHECK_NULL(prefix);

    mcc_core_string mcc_prefix = {.data = prefix, .len = strlen(prefix)};

    return mcc_core_string_starts_with(self, &mcc_prefix);
}

// hashing - FNV-1a
uint64_t mcc_core_string_hash(mcc_core_string* self) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(self);

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
    MCC_CORE_ERROR_CHECK_NULL(arena);
    MCC_CORE_ERROR_CHECK_NULL(self);
    // begin == 0 is perfectly valid
    // len == 0 is perfectly valid (empty string is a substring of all valid strings)

    // 1. Check if 'begin' itself is out of bounds
    if (begin > self->len) {
        mcc_core_error_fatal("%s: 'begin' index out of bounds", __func__);
    }

    // 2. Check if 'len' goes past the end (Safe subtraction prevents overflow)
    if (len > self->len - begin) {
        mcc_core_error_fatal("%s: len exceeds string bounds", __func__);
    }

    // 3. Delegate to the range constructor (This correctly copies/Owns the memory)
    // len == 0 is handled inside the constructor safely
    char* start = self->data + begin;
    return mcc_core_string_construct_copy(arena, start, len);
}

mcc_core_string* mcc_core_string_trim_whitespace(mcc_core_arena* arena, mcc_core_string* self) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(arena);
    MCC_CORE_ERROR_CHECK_NULL(self);

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
    return mcc_core_string_construct_copy(arena, self->data + begin, len);
}

void mcc_core_string_print(mcc_core_string* self, FILE* stream) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(self);
    MCC_CORE_ERROR_CHECK_NULL(stream);

    fwrite(self->data, 1, self->len, stream);
}

void mcc_core_string_println(mcc_core_string* self, FILE* stream) {
    // debug safety
    MCC_CORE_ERROR_CHECK_NULL(self);
    MCC_CORE_ERROR_CHECK_NULL(stream);

    fwrite(self->data, 1, self->len, stream);
    fputc('\n', stream);
}

char* mcc_core_string_get_cstr(mcc_core_string* self) {
    MCC_CORE_ERROR_CHECK_NULL(self);

    return self->data;
}