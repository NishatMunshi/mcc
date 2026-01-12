#include "linux.h"

#define LINUX_SYSCALL_WRITE 1
#define LINUX_SYSCALL_EXIT 60
#define LINUX_SYSCALL_BRK 12

#define LINUX_SYSCALL_READ 0
#define LINUX_SYSCALL_OPEN 2
#define LINUX_SYSCALL_CLOSE 3
#define LINUX_SYSCALL_FSTAT 5

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
    _linux_syscall((s64)code, 0, 0, 0, 0, 0, LINUX_SYSCALL_EXIT);
    while(true);
}

s64 linux_write(s32 fd, char* buf, size_t len) {
    return _linux_syscall((s64)fd, (s64)buf, (s64)len, 0, 0, 0, LINUX_SYSCALL_WRITE);
}

u8* linux_brk(u8* ptr) {
    return (u8*)_linux_syscall((s64)ptr, 0, 0, 0, 0, 0, LINUX_SYSCALL_BRK);
}

s32 linux_open(char* filename, s32 flags, s32 mode) {
    // RDI: filename
    // RSI: flags
    // RDX: mode (Permissions, e.g. 0644. Only used if creating a file)
    return (s32)_linux_syscall(
        (s64)filename,
        (s64)flags,
        (s64)mode,
        0, 0, 0,
        LINUX_SYSCALL_OPEN
    );
}

s64 linux_close(s32 fd) {
    return _linux_syscall((s64)fd, 0, 0, 0, 0, 0, LINUX_SYSCALL_CLOSE);
}

s64 linux_read(s32 fd, char* buf, size_t len) {
    // Note: Syscall expects FD in RDI, Buf in RSI.
    return _linux_syscall(
        (s64)fd,
        (s64)buf,
        (s64)len, 0, 0, 0,
        LINUX_SYSCALL_READ
    );
}

s64 linux_fstat(s32 fd, linux_stat_t* stat) {
    return _linux_syscall((s64)fd, (s64)stat, 0, 0, 0, 0, LINUX_SYSCALL_FSTAT);
}
