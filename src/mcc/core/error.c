#include "mcc/core/error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void mcc_core_error_fatal(char* format, ...) {
    if (format == NULL) {
        fprintf(stderr, "%s: format is NULL", __func__);
        exit(1);
    }

    va_list args;
    va_start(args, format);

    fprintf(stderr, "mcc: fatal error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);

    exit(1);
}