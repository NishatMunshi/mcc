#ifndef _STDDEF_H
#define _STDDEF_H

// 1. NULL - The null pointer constant
#define NULL ((void*)0)

// 2. size_t - The result of the sizeof operator
// On Linux x86-64, this is an unsigned 64-bit integer.
typedef unsigned long size_t;

// 3. ptrdiff_t - The result of subtracting two pointers
// On Linux x86-64, this is a signed 64-bit integer.
typedef long ptrdiff_t;

// 4. wchar_t - Wide character type
// Usually a 32-bit int on Linux (UTF-32).
#if !defined(__INTELLISENSE__) || !defined(__WCHAR_TYPE__)
typedef int wchar_t;
#endif

// 5. offsetof - The byte offset of a member in a struct
// This "trick" casts 0 to a pointer of type 'type', then takes the address of 'member'.
// Since the base is 0, the address of the member is its offset.
#define offsetof(type, member) ((size_t)&(((type*)0)->member))

// 6. max_align_t (Required by C11)
// We create a struct containing the widest types to force the compiler
// to calculate the maximum necessary alignment (usually 16 bytes on x64).
typedef union {
    long long __max_align_ll;
    long double __max_align_ld;
} max_align_t;

#endif
