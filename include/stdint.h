#ifndef _STDINT_H
#define _STDINT_H

// -----------------------------------------------------------------------------
// 1. Integer Types (C11 7.20.1)
// -----------------------------------------------------------------------------

// 7.20.1.1 Exact-width integer types
// (Required because x86-64 supports 8, 16, 32, and 64-bit twos-complement)
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long               int64_t;

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long      uint64_t;

// 7.20.1.2 Minimum-width integer types
typedef int8_t             int_least8_t;
typedef int16_t            int_least16_t;
typedef int32_t            int_least32_t;
typedef int64_t            int_least64_t;

typedef uint8_t            uint_least8_t;
typedef uint16_t           uint_least16_t;
typedef uint32_t           uint_least32_t;
typedef uint64_t           uint_least64_t;

// 7.20.1.3 Fastest minimum-width integer types
// On x64, 64-bit integers are technically the native register size.
typedef int8_t             int_fast8_t;
typedef int64_t            int_fast16_t;
typedef int64_t            int_fast32_t;
typedef int64_t            int_fast64_t;

typedef uint8_t            uint_fast8_t;
typedef uint64_t           uint_fast16_t;
typedef uint64_t           uint_fast32_t;
typedef uint64_t           uint_fast64_t;

// 7.20.1.4 Integer types capable of holding object pointers
typedef long               intptr_t;
typedef unsigned long      uintptr_t;

// 7.20.1.5 Greatest-width integer types
typedef long               intmax_t;
typedef unsigned long      uintmax_t;


// -----------------------------------------------------------------------------
// 2. Limits of Specified-Width Integer Types (C11 7.20.2)
// -----------------------------------------------------------------------------

// 7.20.2.1 Limits of exact-width integer types
#define INT8_MIN         (-128)
#define INT16_MIN        (-32768)
#define INT32_MIN        (-2147483648)
#define INT64_MIN        (-9223372036854775807L - 1)

#define INT8_MAX         (127)
#define INT16_MAX        (32767)
#define INT32_MAX        (2147483647)
#define INT64_MAX        (9223372036854775807L)

#define UINT8_MAX        (255)
#define UINT16_MAX       (65535)
#define UINT32_MAX       (4294967295U)
#define UINT64_MAX       (18446744073709551615UL)

// 7.20.2.2 Limits of minimum-width integer types
#define INT_LEAST8_MIN   INT8_MIN
#define INT_LEAST16_MIN  INT16_MIN
#define INT_LEAST32_MIN  INT32_MIN
#define INT_LEAST64_MIN  INT64_MIN

#define INT_LEAST8_MAX   INT8_MAX
#define INT_LEAST16_MAX  INT16_MAX
#define INT_LEAST32_MAX  INT32_MAX
#define INT_LEAST64_MAX  INT64_MAX

#define UINT_LEAST8_MAX  UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

// 7.20.2.3 Limits of fastest minimum-width integer types
#define INT_FAST8_MIN    INT8_MIN
#define INT_FAST16_MIN   INT64_MIN
#define INT_FAST32_MIN   INT64_MIN
#define INT_FAST64_MIN   INT64_MIN

#define INT_FAST8_MAX    INT8_MAX
#define INT_FAST16_MAX   INT64_MAX
#define INT_FAST32_MAX   INT64_MAX
#define INT_FAST64_MAX   INT64_MAX

#define UINT_FAST8_MAX   UINT8_MAX
#define UINT_FAST16_MAX  UINT64_MAX
#define UINT_FAST32_MAX  UINT64_MAX
#define UINT_FAST64_MAX  UINT64_MAX

// 7.20.2.4 Limits of integer types capable of holding object pointers
#define INTPTR_MIN       INT64_MIN
#define INTPTR_MAX       INT64_MAX
#define UINTPTR_MAX      UINT64_MAX

// 7.20.2.5 Limits of greatest-width integer types
#define INTMAX_MIN       INT64_MIN
#define INTMAX_MAX       INT64_MAX
#define UINTMAX_MAX      UINT64_MAX


// -----------------------------------------------------------------------------
// 3. Limits of Other Integer Types (C11 7.20.3)
// -----------------------------------------------------------------------------

// ptrdiff_t limits
#define PTRDIFF_MIN      INT64_MIN
#define PTRDIFF_MAX      INT64_MAX

// sig_atomic_t limits (usually int on Linux)
#define SIG_ATOMIC_MIN   INT32_MIN
#define SIG_ATOMIC_MAX   INT32_MAX

// size_t limit
#define SIZE_MAX         UINT64_MAX

// wchar_t limits (Linux uses 32-bit signed int for wchar_t)
#define WCHAR_MIN        INT32_MIN
#define WCHAR_MAX        INT32_MAX

// wint_t limits (Usually 32-bit unsigned int)
#define WINT_MIN         0U
#define WINT_MAX         UINT32_MAX


// -----------------------------------------------------------------------------
// 4. Macros for Integer Constants (C11 7.20.4)
// -----------------------------------------------------------------------------

// 7.20.4.1 Macros for minimum-width integer constants
#define INT8_C(x)        x
#define INT16_C(x)       x
#define INT32_C(x)       x
#define INT64_C(x)       x ## L

#define UINT8_C(x)       x
#define UINT16_C(x)      x
#define UINT32_C(x)      x ## U
#define UINT64_C(x)      x ## UL

// 7.20.4.2 Macros for greatest-width integer constants
#define INTMAX_C(x)      x ## L
#define UINTMAX_C(x)     x ## UL

#endif
