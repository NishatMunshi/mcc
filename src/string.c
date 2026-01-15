#include "string.h"

size_t strlen(char* cstr) {
    size_t len = 0;
    while (cstr[len]) len++;
    return len;
}

bool streq(char* a, char* b) {
    for (; *a == *b; a++, b++)
        if (!*a) return true;
    return false;
}

void memcpy(void* dest, void* src, size_t num_bytes) {
    u8* d = (u8*)dest;
    u8* s = (u8*)src;
    for (size_t i = 0; i < num_bytes; ++i) {
        d[i] = s[i];
    }
}
