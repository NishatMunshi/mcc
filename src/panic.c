#include <io.h>
#include <linux.h>
#include <panic.h>

[[noreturn]] void panic(char* message) {
    printf("mcc: error: %s\n", message);
    linux_exit(LINUX_EXIT_FAILURE);
}
