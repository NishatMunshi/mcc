#ifndef TYPES_H
#define TYPES_H

typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned char u8;

typedef long long s64;
typedef int s32;

typedef u64 size_t;

typedef union {
    long long ll;
    long double ld;
} max_align_t;

#endif
