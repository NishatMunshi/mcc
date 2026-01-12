#ifndef IOSTREAM_H
#define IOSTREAM_H

#include "linux.h"
#include "types.h"

#define IOSTREAM_STDOUT LINUX_FD_STDOUT
#define IOSTREAM_STDERR LINUX_FD_STDERR

s64 iostream_print_str(s32 stream, char* cstr);
s64 iostream_print_uint(s32 stream, u64 num);

#endif  // IOSTREAM_H
