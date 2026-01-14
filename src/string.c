#include "string.h"

size_t strlen(char* cstr) {
    size_t len = 0;
    while (cstr[len++]);
    return len;
}

bool streq(char* a, char* b) {
    for (; *a == *b; a++, b++)
        if (!*a) return true;
    return false;
}
