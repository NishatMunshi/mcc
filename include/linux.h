#ifndef LINUX_H
#define LINUX_H

#include "types.h"

#define LINUX_EXIT_FAILURE ((u8)1)
#define LINUX_EXIT_SUCCESS ((u8)0)

#define LINUX_FD_STDOUT ((s32)1)
#define LINUX_FD_STDERR ((s32)2)

#define LINUX_BRK_CURRENT ((u8*)0)

#define LINUX_FILE_FLAG_READONLY 0
#define LINUX_FILE_FLAG_WRITEONLY 1
#define LINUX_FILE_FLAG_READWRITE 2
#define LINUX_FILE_FLAG_CREAT 64

#define LINUX_FILE_MODE_USER_RW 438

[[noreturn]] void linux_exit(u8 code);
s64 linux_write(s32 fd, char* buf, size_t len);
// guaraantees zeroed out pages on first request
// which will be our only request since we dont free
u8* linux_brk(u8* ptr);

typedef struct {
    u64 st_dev;
    u64 st_ino;
    u64 st_nlink;
    u32 st_mode;
    u32 st_uid;
    u32 st_gid;
    u32 __pad0;
    u64 st_rdev;
    s64 st_size; /* We need this for file size */
    s64 st_blksize;
    s64 st_blocks;
    s64 st_atime_sec;
    s64 st_atime_nsec;
    s64 st_mtime_sec;
    s64 st_mtime_nsec;
    s64 st_ctime_sec;
    s64 st_ctime_nsec;
    s64 __unused[3];
} linux_stat_t;

s32 linux_open(char* filename, s32 flags, s32 mode);
s64 linux_close(s32 fd);
s64 linux_read(s32 fd, u8* buf, size_t len);
s64 linux_fstat(s32 fd, linux_stat_t* stat);

#endif  //  LINUX_H
