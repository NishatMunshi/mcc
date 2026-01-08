#ifndef MCC_CORE_STRING_H
#define MCC_CORE_STRING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "mcc/core/arena.h"

/**
 * @brief Immutable, len-prefixed string slice.
 *
 * This structure represents a managed string of characters.
 * It is designed to be allocated within an `mcc_core_arena`.
 * len is the number of characters excluding any null at the end
 * len == 0 is allowed
 *
 * Unlike standard C strings, these strings:
 * 1. Store their len explicitly, allowing for O(1) len checks.
 * 2. Are generally immutable once constructed.
 * 3. May or may not be null-terminated internally (dependant on implementation),
 * so always use the provided print/compare functions rather than raw C functions.
 *
 * @note ERROR HANDLING: This module uses "Infallible Allocation".
 * If an arena allocation fails during string construction or manipulation,
 * the program terminates immediately via mcc_core_error.h.
 */
typedef struct mcc_core_string mcc_core_string;

/* --- Constructors --- */

/**
 * @brief Creates a new string from a specific memory range.
 *
 * Deep copies `len` bytes starting from `start` into the provided `arena`.
 * This is useful for creating strings from buffers that are not null-terminated.
 *
 * @param arena The arena to allocate the new string data into.
 * @param start Pointer to the start of the character data.
 * @param len The number of bytes to copy.
 * @return A new string instance containing a copy of the range (Never NULL).
 */
mcc_core_string* mcc_core_string_construct_copy(mcc_core_arena* arena, char* start, size_t len);

/**
 * @brief Creates a new string by copying a standard null-terminated C string.
 *
 * Deep copies the content of `cstr` into the provided `arena`.
 *
 * @param arena The arena to allocate the new string data into.
 * @param cstr A null-terminated C string. Must not be NULL.
 * @return A new string instance containing a copy of the data (Never NULL).
 */
mcc_core_string* mcc_core_string_construct_copy_from_cstr(mcc_core_arena* arena, char* cstr);

/**
 * @brief Creates a string object taking ownership of an existing buffer.
 *
 * Allocates the string metadata struct in the arena, but points directly
 * to the provided `cstr` buffer without copying.
 *
 * @param arena The arena to allocate the string struct into.
 * @param cstr  The existing buffer to use. Must be a valid pointer.
 * This function assumes it can modify `cstr[len]` for termination.
 * @param len   The length of the string (excluding null terminator).
 * @return A new string instance wrapping the provided buffer.
 */
mcc_core_string* mcc_core_string_construct_move_from_cstr(mcc_core_arena* arena, char* cstr, size_t len);

/* --- Comparators --- */

/**
 * @brief Checks if two mcc strings are identical.
 *
 * @param self The first string.
 * @param other The second string.
 * @return true if lens match and byte content is identical, false otherwise.
 */
bool mcc_core_string_equals(mcc_core_string* self, mcc_core_string* other);

/**
 * @brief Checks if an mcc string matches a standard C string.
 *
 * @param self The mcc string.
 * @param cstr A null-terminated C string.
 * @return true if the mcc string matches the content of the cstr.
 */
bool mcc_core_string_equals_cstr(mcc_core_string* self, char* cstr);

/**
 * @brief Checks if the string starts with the specified prefix.
 *
 * @param self The string to check.
 * @param prefix The prefix string to look for.
 * @return true if `self` begins with the exact sequence of `prefix`.
 */
bool mcc_core_string_starts_with(mcc_core_string* self, mcc_core_string* prefix);

/**
 * @brief Checks if the string starts with the specified prefix.
 *
 * @param self The string to check.
 * @param prefix The prefix string to look for. (c string)
 * @return true if `self` begins with the exact sequence of `prefix`.
 */
bool mcc_core_string_starts_with_cstr(mcc_core_string* self, char* prefix);

/* --- Hashing --- */

/**
 * @brief Generates a 64-bit hash of the string content.
 *
 * Useful for storing strings in hash maps or sets.
 * Implementation uses FNV-1a.
 *
 * @param self The string to hash.
 * @return A 64-bit hash value.
 */
uint64_t mcc_core_string_hash(mcc_core_string* self);

/* --- Utilities --- */

/**
 * @brief Creates a new string containing a portion of the original.
 *
 * This performs a deep copy of the sub-range into the arena.
 *
 * @param arena The arena to allocate the new substring into.
 * @param self The source string.
 * @param begin The zero-based start index (inclusive).
 * @param len The len of the required substring.
 * @return A new string containing `len` characters starting from `begin` (Never NULL).
 */
mcc_core_string* mcc_core_string_substring(mcc_core_arena* arena, mcc_core_string* self, size_t begin, size_t len);

/**
 * @brief Creates a new string with leading and trailing whitespace removed.
 *
 * Allocates a new string in the arena containing the trimmed content.
 * Whitespace is typically defined as space, tab, newline, etc.
 *
 * @param arena The arena to allocate the new trimmed string into.
 * @param self The source string.
 * @return A new string with whitespace stripped from both ends (Never NULL).
 */
mcc_core_string* mcc_core_string_trim_whitespace(mcc_core_arena* arena, mcc_core_string* self);

/**
 * @brief Prints the string content to the specified output stream.
 *
 * @param self The string to print.
 * @param stream The file stream (e.g., stdout, stderr).
 */
void mcc_core_string_print(mcc_core_string* self, FILE* stream);

/**
 * @brief Prints the string content followed by a newline to the output stream.
 *
 * @param self The string to print.
 * @param stream The file stream (e.g., stdout, stderr).
 */
void mcc_core_string_println(mcc_core_string* self, FILE* stream);

/**
 * @brief gets the character pointer to the start of the string
 *
 * Null terminates the string properly before return
 *
 * @param self pointer to the string object
 * @return the character pointer to the start of the string
 */
char* mcc_core_string_get_cstr(mcc_core_string* self);

size_t mcc_core_string_get_len(mcc_core_string* self);

char mcc_core_string_at(mcc_core_string* self, size_t idx);

#endif  // MCC_CORE_STRING_H