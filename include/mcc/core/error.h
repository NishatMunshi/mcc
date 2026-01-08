#ifndef MCC_CORE_ERROR_H
#define MCC_CORE_ERROR_H

void mcc_core_error_fatal(char *format, ...);

/**
 * @brief Checks if a pointer is NULL. 
 * Automatically captures the current function name.
 */
#define MCC_CORE_ERROR_CHECK_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            mcc_core_error_fatal("%s: %s is NULL", __func__, #ptr); \
        } \
    } while (0)

#endif // MCC_CORE_ERROR_H