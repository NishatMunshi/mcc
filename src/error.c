#include "error.h"
#include "linux.h"
#include "string.h"

noreturn void error_fatal(char* message) {
    char prefix[] = "mcc: error: ";
    size_t prefix_len = sizeof(prefix) - 1;
    size_t msg_len = str_len(message);

    linux_write(LINUX_FD_STDERR, prefix, prefix_len);
    linux_write(LINUX_FD_STDERR, message, msg_len);
    linux_write(LINUX_FD_STDERR, "\n", 1);
    linux_exit(LINUX_EXIT_FAILURE);

    // trap to satisfy noreturn
    while(true);
}
