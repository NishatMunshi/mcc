#include <arena.h>
#include <string.h>

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

char* strcat(char* cstr1, char* cstr2) {
    size_t len1 = strlen(cstr1);
    size_t len2 = strlen(cstr2);

    char* buf = ARENA_ALLOC(char, len1 + len2 + 1);

    for (size_t i = 0; i < len1; i++) {
        buf[i] = cstr1[i];
    }

    for (size_t i = 0; i < len2; ++i) {
        buf[i + len1] = cstr2[i];
    }

    return buf;
}

char* strrchr(char* cstr, char c) {
    for (s64 i = strlen(cstr) - 1; i >= 0; --i) {
        if (cstr[i] == c) return cstr + i;
    }
    return nullptr;
}

char* strdup(char* cstr) {
    size_t len = strlen(cstr);
    char* buf = ARENA_ALLOC(char, len + 1);
    memcpy(buf, cstr, len);
    buf[len] = '\0';
    return buf;
}

void memcpy(void* dest, void* src, size_t num_bytes) {
    u8* d = (u8*)dest;
    u8* s = (u8*)src;
    for (size_t i = 0; i < num_bytes; ++i) {
        d[i] = s[i];
    }
}

void* memset(void* ptr, int value, size_t num) {
    u8* dest = (u8*)ptr;
    u8 data = (u8)(value & 0xff);

    for (size_t i = 0; i < num; ++i) {
        dest[i] = data;
    }
    return ptr;
}
