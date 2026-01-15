#include <panic.h>

#include <io.h>
#include <linux.h>

[[noreturn]] void panic(char* message) {
    fputs(stderr, "mcc: error: ");
    fputs(stderr, message);
    fputs(stderr, "\n");
    linux_exit(LINUX_EXIT_FAILURE);
}
