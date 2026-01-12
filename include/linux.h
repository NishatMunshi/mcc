#ifndef LINUX_H
#define LINUX_H

#include "types.h"
#include "stdnoreturn.h"

#define LINUX_EXIT_FAILURE ((u8)1)
#define LINUX_EXIT_SUCCESS ((u8)0)

noreturn void linux_exit(u8 code);

#define LINUX_FD_STDOUT ((s32)1)
#define LINUX_FD_STDERR ((s32)2)

void linux_write(s32 fd, char* buf, size_t len);

#define LINUX_BRK_CURRENT ((u8*)0)
// guaraantees zeroed out pages on first request
// which will be our only request since we dont free
u8* linux_brk(u8* ptr);

#if 0
s64 linux_file_open(char* filename);
s64 linux_file_close(s64 fd);
s64 linux_file_read(char* buf, s64 fd, s64 len);
s64 linux_file_size(s64 fd);
#endif

#endif  //  LINUX_H
