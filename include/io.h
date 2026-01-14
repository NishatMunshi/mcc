#ifndef _STDIO_H
#define _STDIO_H

#include "linux.h"
#include "types.h"

#define stdout LINUX_FD_STDOUT
#define stderr LINUX_FD_STDERR

s64 fputc(s32 stream, char c);
s64 fputu(s32 stream, u64 num);
s64 fputs(s32 stream, char* cstr);

s64 putc(char c);
s64 putu(u64 num);
s64 puts(char* cstr);

#endif  // _STDIO_H
