#ifndef MCC_CORE_ERROR_H
#define MCC_CORE_ERROR_H

#include <assert.h>

void mcc_core_error_fatal(char* format, ...);

/**
 * @brief Checks if a pointer is NULL.
 * Automatically captures the current function name.
 */
#define MCC_CORE_ERROR_FATAL_CHECK_NULL(ptr)                        \
    do {                                                            \
        if ((ptr) == NULL) {                                        \
            mcc_core_error_fatal("%s: %s is NULL", __func__, #ptr); \
        }                                                           \
    } while (0)

/**
 * @brief Checks if a variable is ZERO.
 * Automatically captures the current function name.
 */
#define MCC_CORE_ERROR_FATAL_CHECK_ZERO(var)                        \
    do {                                                            \
        if ((var) == 0) {                                           \
            mcc_core_error_fatal("%s: %s is ZERO", __func__, #var); \
        }                                                           \
    } while (0)
#endif  // MCC_CORE_ERROR_H

#define MCC_CORE_ERROR_UNREACHABLE() assert(false && "UNREACHABLE")