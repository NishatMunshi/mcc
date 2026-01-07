#include "mcc/core/string.h"

#include <ctype.h>
#include <string.h>

#include "mcc/core/arena.h"
#include "mcc/core/error.h"

struct mcc_core_string {
    char* data;
    size_t length;
};

// constructors
mcc_core_string* mcc_core_string_construct_from_c_string(mcc_core_arena* arena, char* c_string) {
    mcc_core_string* string = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_string, 1);

    string->data = c_string;
    string->length = strlen(c_string);

    return string;
}

mcc_core_string* mcc_core_string_construct_from_range(mcc_core_arena* arena, char* start, size_t length) {
    mcc_core_string* string = MCC_CORE_ARENA_ALLOCATE(arena, mcc_core_string, 1);

    string->data = start;
    string->length = length;

    return string;
}

// comparators
bool mcc_core_string_equals_string(const mcc_core_string* self, const mcc_core_string* other) {
    if (self->length != other->length) {
        return false;
    }

    return !memcmp(self->data, other->data, self->length);
}

bool mcc_core_string_equals_c_string(const mcc_core_string* self, const char* c_string) {
    size_t c_string_length = strlen(c_string);

    if (self->length != c_string_length) {
        return false;
    }

    return !memcmp(self->data, c_string, self->length);
}

bool mcc_core_string_starts_with(const mcc_core_string* self, const mcc_core_string* prefix) {
    if (self->length < prefix->length) {
        return false;
    }

    for (size_t i = 0; i < prefix->length; ++i) {
        if (self->data[i] != prefix->data[i]) {
            return false;
        }
    }

    return true;
}

// hashing - FNV-1a
uint64_t mcc_core_string_hash(const mcc_core_string* self) {
    uint64_t hash = 0xcbf29ce484222325;

    for (size_t i = 0; i < self->length; ++i) {
        hash = hash ^ (uint8_t)(self->data[i]);
        hash = hash * 0x100000001b3;
    }

    return hash;
}

// utilities
mcc_core_string* mcc_core_string_substring(mcc_core_arena* arena, const mcc_core_string* self, size_t begin, size_t length) {
    if (self->length < begin + length) {
        mcc_core_error_fatal("substring out of bound");
    }

    char* start = self->data + begin;
    return mcc_core_string_construct_from_range(arena, start, length);
}

static mcc_core_string* _mcc_core_string_construct_empty(mcc_core_arena* arena) {
    return mcc_core_string_construct_from_range(arena, NULL, 0);
}

static bool _mcc_core_string_is_empty(const mcc_core_string* self) {
    return self->length == 0;
}

mcc_core_string* mcc_core_string_trim_whitespace(mcc_core_arena* arena, const mcc_core_string* self) {
    if (_mcc_core_string_is_empty(self)) {
        return _mcc_core_string_construct_empty(arena);
    }

    size_t begin = 0;
    size_t length = self->length;

    // trim beginning
    while (length > 0 && isspace(self->data[begin])) {
        begin++;
        length--;
    }

    // trim end
    while (length > 0 && isspace(self->data[begin + length - 1])) {
        length--;
    }

    return mcc_core_string_construct_from_range(arena, self->data + begin, length);
}

void mcc_core_string_print(const mcc_core_string* self, FILE* output_stream) {
    for (size_t i = 0; i < self->length; ++i) {
        char character = self->data[i];
        fprintf(output_stream, "%c", character);
    }
}

void mcc_core_string_println(const mcc_core_string* self, FILE* output_stream) {
    for (size_t i = 0; i < self->length; ++i) {
        char character = self->data[i];
        fprintf(output_stream, "%c", character);
    }
    fprintf(output_stream, "\n");
}
