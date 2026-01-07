#ifndef MCC_CORE_STRING_H
#define MCC_CORE_STRING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "mcc/core/arena.h"

typedef struct mcc_core_string mcc_core_string;

// constructors
mcc_core_string* mcc_core_string_construct_from_c_string(mcc_core_arena* arena, char* c_string);
mcc_core_string* mcc_core_string_construct_from_range(mcc_core_arena* arena, char* start, size_t length);

// comparators
bool mcc_core_string_equals_string(const mcc_core_string* self, const mcc_core_string* other);
bool mcc_core_string_equals_c_string(const mcc_core_string* self, const char* c_string);
bool mcc_core_string_starts_with(const mcc_core_string* self, const mcc_core_string* prefix);

// hashing
uint64_t mcc_core_string_hash(const mcc_core_string* self);

// utilities
mcc_core_string* mcc_core_string_substring(mcc_core_arena* arena, const mcc_core_string* self, size_t begin, size_t end);
mcc_core_string* mcc_core_string_trim_whitespace(mcc_core_arena* arena, const mcc_core_string* self);
void mcc_core_string_print(const mcc_core_string* self, FILE* output_stream);
void mcc_core_string_println(const mcc_core_string* self, FILE* output_stream);

#endif  // MCC_CORE_STRING_H