#include "mcc/core/error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void mcc_core_error_fatal(char *format, ...) {
    MCC_CORE_ERROR_CHECK_NULL(format);
    
    va_list args;
    va_start(args, format);

    fprintf(stderr, "mcc: fatal error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);

    exit(1);
}