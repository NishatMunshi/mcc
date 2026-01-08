#ifndef MCC_CORE_FILE_H
#define MCC_CORE_FILE_H

#include "mcc/core/arena.h"
#include "mcc/core/string.h"

/**
 * @file file.h
 * @brief File I/O Utilities.
 *
 * Provides functions to read files entirely into memory using the
 * arena allocator for storage.
 */

/**
 * @brief Reads an entire file into a string in the given arena.
 *
 * Opens the specified file in binary mode, determines its size,
 * and reads the full content into a buffer allocated from the provided arena.
 *
 * @param arena      The memory arena used to allocate the file buffer.
 * @param file_name  The path to the file to read.
 * * @return A string containing the file data.
 * * @note This function is **Fatal**. If the file cannot be opened, read,
 * or if size determination fails, the program will print an error
 * to stderr and exit(1). It never returns NULL.
 * * @warning The returned string data is guaranteed to be null-terminated,
 * making it safe to use with C-style APIs if needed.
 */
mcc_core_string* mcc_core_file_read(mcc_core_arena* arena, mcc_core_string* file_name);

#endif  // MCC_CORE_FILE_H