#include "error.h"

#include "iostream.h"
#include "string.h"

noreturn void error_fatal(char* message) {
    char* prefix = "mcc: error: ";

    iostream_print_str(IOSTREAM_STDERR, prefix);
    iostream_print_str(IOSTREAM_STDERR, message);
    iostream_print_str(IOSTREAM_STDERR, "\n");

    linux_exit(LINUX_EXIT_FAILURE);

    // trap to satisfy noreturn
    while (true);
}
