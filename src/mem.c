#include "mem.h"

#include "linux.h"

void mem_copy(void* dst, void* src, size_t num_bytes) {
    u8* d = (u8*)dst;
    u8* s = (u8*)src;

    for (size_t i = 0; i < num_bytes; ++i) {
        d[i] = s[i];
    }
}
