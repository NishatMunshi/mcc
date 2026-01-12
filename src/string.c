#include "string.h"

size_t str_len(char* cstr) {
    size_t len = 0;
    while (cstr[len]) len++;
    return len;
}
