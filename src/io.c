#include "io.h"

#include "string.h"

s64 fputc(s32 stream, char c) {
    return linux_write(stream, &c, 1);
}

s64 fputu(s32 stream, u64 num) {
    u64 pref = num / 10;

    s64 num_written = 0;
    if (pref != 0) {
        num_written += fputu(stream, pref);
    }

    char c = '0' + (num % 10);
    return num_written + fputc(stream, c);
}

s64 fputs(s32 stream, char* cstr) {
    return linux_write(stream, cstr, strlen(cstr));
}

s64 putc(char c) {
    return fputc(stdout, c);
}

s64 putu(u64 num) {
    return fputu(stdout, num);
}

s64 puts(char* cstr) {
    return fputs(stdout, cstr);
}
