#include "iostream.h"

#include "string.h"

s64 iostream_print_str(s32 stream, char* cstr) {
    size_t len = str_len(cstr);
    return linux_write(stream, cstr, len);
}

s64 iostream_print_uint(s32 stream, u64 num) {
    size_t pref = num / 10;
    if (pref) {
        iostream_print_uint(stream, pref);
    }

    char c = '0' + (num % 10);
    return linux_write(stream, &c, 1);
}
