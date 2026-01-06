#include "mcc/core/error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void mcc_core_error_fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(stderr, "[FATAL]: ");
    vprintf(format, args);

    va_end(args);
    exit(1);
}

// void mcc_core_error_report() {}