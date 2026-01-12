#include "linux.h"

#define _LINUX_SYSCALL_WRITE ((s32)1)
#define _LINUX_SYSCALL_EXIT ((s32)60)
#define _LINUX_SYSCALL_BRK ((s32)12)

#define LINUX_SYSCALL_READ 0
#define LINUX_SYSCALL_OPEN 2
#define LINUX_SYSCALL_CLOSE 3
#define LINUX_SYSCALL_FSTAT 5

#define LINUX_FILE_MODE_READONLY 0
#define LINUX_FILE_MODE_WRITEONLY 1
#define LINUX_FILE_MODE_READWRITE 2

extern s64 _linux_syscall(
    s64 rdi,  // C puts it in: rdi
    s64 rsi,  // C puts it in: rsi
    s64 rdx,  // C puts it in: rdx
    s64 r10,  // C puts it in: rcx
    s64 r8,   // C puts it in: r8
    s64 r9,   // C puts it in: r9
    s64 rax   // C puts it in: [rsp + 8]
);

noreturn void linux_exit(u8 code) {
    _linux_syscall((s64)code, 0, 0, 0, 0, 0, _LINUX_SYSCALL_EXIT);
    while(true);
}

void linux_write(s32 fd, char* buf, size_t len) {
    _linux_syscall((s64)fd, (s64)buf, (s64)len, 0, 0, 0, _LINUX_SYSCALL_WRITE);
}

u8* linux_brk(u8* ptr) {
    return (u8*)_linux_syscall((s64)ptr, 0, 0, 0, 0, 0, _LINUX_SYSCALL_BRK);
}

#if 0

s64 linux_file_open(char* filename) {
    // RDI: filename, RSI: flags, RDX: mode (ignored for read)
    return _linux_syscall(
        (s64)filename,
        LINUX_FILE_MODE_READONLY,
        0, 0, 0, 0,
        LINUX_SYSCALL_OPEN
    );
}

s64 linux_file_close(s64 fd) {
    return _linux_syscall(fd, 0, 0, 0, 0, 0, LINUX_SYSCALL_CLOSE);
}

s64 linux_file_read(char* buf, s64 fd, s64 len) {
    // Note: Syscall expects FD in RDI, Buf in RSI.
    return _linux_syscall(
        fd,
        (s64)buf,
        len, 0, 0, 0,
        LINUX_SYSCALL_READ
    );
}

/* The Kernel Stat Structure (Must match x86-64 layout) */
struct kernel_stat {
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
};

s64 linux_file_size(s64 fd) {
    struct kernel_stat statbuf;

    // Syscall 5 (fstat) fills the statbuf structure
    s64 result = _linux_syscall(
        fd,
        (s64)&statbuf,
        0, 0, 0, 0,
        LINUX_SYSCALL_FSTAT
    );

    if (result < 0) {
        return result;  // Propagate error
    }

    return statbuf.st_size;
}

#endif
