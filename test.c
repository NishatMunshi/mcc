
#ifndef TYPES_H
#define TYPES_H

// strict standards
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

// lazy short types for this project
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#endif

#ifndef _STDNORETURN_H
#define _STDNORETURN_H

#define noreturn _Noreturn

#define __noreturn_is_defined 1

#endif  // _STDNORETURN_H

#ifndef TOKEN_H
#define TOKEN_H

#include "stdnoreturn.h"
#include "types.h"

typedef enum TokenType {
    // --- Internal Control ---
    TOK_EOF,    // End of File
    TOK_ERROR,  // Lexical Error

    // --- C11 Keywords (6.4.1) ---
    TOK_AUTO,           // auto
    TOK_BREAK,          // break
    TOK_CASE,           // case
    TOK_CHAR,           // char
    TOK_CONST,          // const
    TOK_CONTINUE,       // continue
    TOK_DEFAULT,        // default
    TOK_DO,             // do
    TOK_DOUBLE,         // double
    TOK_ELSE,           // else
    TOK_ENUM,           // enum
    TOK_EXTERN,         // extern
    TOK_FLOAT,          // float
    TOK_FOR,            // for
    TOK_GOTO,           // goto
    TOK_IF,             // if
    TOK_INLINE,         // inline
    TOK_INT,            // int
    TOK_LONG,           // long
    TOK_REGISTER,       // register
    TOK_RESTRICT,       // restrict
    TOK_RETURN,         // return
    TOK_SHORT,          // short
    TOK_SIGNED,         // signed
    TOK_SIZEOF,         // sizeof
    TOK_STATIC,         // static
    TOK_STRUCT,         // struct
    TOK_SWITCH,         // switch
    TOK_TYPEDEF,        // typedef
    TOK_UNION,          // union
    TOK_UNSIGNED,       // unsigned
    TOK_VOID,           // void
    TOK_VOLATILE,       // volatile
    TOK_WHILE,          // while
    TOK_ALIGNAS,        // _Alignas
    TOK_ALIGNOF,        // _Alignof
    TOK_ATOMIC,         // _Atomic
    TOK_BOOL,           // _Bool
    TOK_COMPLEX,        // _Complex
    TOK_GENERIC,        // _Generic
    TOK_IMAGINARY,      // _Imaginary
    TOK_NORETURN,       // _Noreturn
    TOK_STATIC_ASSERT,  // _Static_assert
    TOK_THREAD_LOCAL,   // _Thread_local

    // --- Identifiers (6.4.2) ---
    TOK_IDENTIFIER,  // main, x, my_var

    // --- Constants (6.4.4) ---
    TOK_PP_NUMBER,   // 123, 0xFF, 0777, 3.14, 1.2e-10, 0x1.fp3
    TOK_CONST_CHAR,  // 'a', L'a', u'a', U'a'

    // --- String Literals (6.4.5) ---
    TOK_STRING_LITERAL,  // "abc", L"abc", u8"abc", u"abc", U"abc"

    // --- Punctuators (6.4.6) ---
    TOK_LBRACKET,  // [  and  <:
    TOK_RBRACKET,  // ]  and  :>
    TOK_LPAREN,    // (
    TOK_RPAREN,    // )
    TOK_LBRACE,    // {  and  <%
    TOK_RBRACE,    // }  and  %>
    TOK_DOT,       // .
    TOK_ARROW,     // ->

    TOK_INC,        // ++
    TOK_DEC,        // --
    TOK_AMPERSAND,  // &
    TOK_STAR,       // *
    TOK_PLUS,       // +
    TOK_MINUS,      // -
    TOK_TILDE,      // ~
    TOK_BANG,       // !

    TOK_SLASH,        // /
    TOK_PERCENT,      // %
    TOK_LSHIFT,       // <<
    TOK_RSHIFT,       // >>
    TOK_LESS,         // <
    TOK_GREATER,      // >
    TOK_LESS_EQ,      // <=
    TOK_GREATER_EQ,   // >=
    TOK_EQ_EQ,        // ==
    TOK_NOT_EQ,       // !=
    TOK_CARET,        // ^
    TOK_PIPE,         // |
    TOK_LOGICAL_AND,  // &&
    TOK_LOGICAL_OR,   // ||

    TOK_QUESTION,   // ?
    TOK_COLON,      // :
    TOK_SEMICOLON,  // ;
    TOK_ELLIPSIS,   // ...

    TOK_ASSIGN,         // =
    TOK_MUL_ASSIGN,     // *=
    TOK_DIV_ASSIGN,     // /=
    TOK_MOD_ASSIGN,     // %=
    TOK_ADD_ASSIGN,     // +=
    TOK_SUB_ASSIGN,     // -=
    TOK_LSHIFT_ASSIGN,  // <<=
    TOK_RSHIFT_ASSIGN,  // >>=
    TOK_AND_ASSIGN,     // &=
    TOK_XOR_ASSIGN,     // ^=
    TOK_OR_ASSIGN,      // |=

    TOK_COMMA,       // ,
    TOK_HASH,        // #   and  %:
    TOK_DOUBLE_HASH  // ##  and  %:%:

} TokenType;

typedef struct File File;
typedef struct {
    // I live inside
    File* file;

    // inside line
    size_t line;
    char* line_start;

    // at
    size_t col;
    char* text_start;
    size_t text_len;

    // Therefore I am a
    TokenType type;
} Token;

// naming is cruital for use with
// the macro in vector.h
typedef struct {
    Token* data;
    size_t count;
    size_t cap;
} TokenVector;

#endif  // TOKEN_H

#ifndef _STDALIGN_H
#define _STDALIGN_H

// 1. The Alignment Specifier
// Expands to the C11 keyword _Alignas
#define alignas _Alignas

// 2. The Alignment Operator
// Expands to the C11 keyword _Alignof
#define alignof _Alignof

// 3. Conformance Signals
// Required to allow #ifdef checks
#define __alignas_is_defined 1
#define __alignof_is_defined 1

#endif

#ifndef _STDINT_H
#define _STDINT_H

// -----------------------------------------------------------------------------
// 1. Integer Types (C11 7.20.1)
// -----------------------------------------------------------------------------

// 7.20.1.1 Exact-width integer types
// (Required because x86-64 supports 8, 16, 32, and 64-bit twos-complement)
typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

// 7.20.1.2 Minimum-width integer types
typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

// 7.20.1.3 Fastest minimum-width integer types
// On x64, 64-bit integers are technically the native register size.
typedef int8_t int_fast8_t;
typedef int64_t int_fast16_t;
typedef int64_t int_fast32_t;
typedef int64_t int_fast64_t;

typedef uint8_t uint_fast8_t;
typedef uint64_t uint_fast16_t;
typedef uint64_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

// 7.20.1.4 Integer types capable of holding object pointers
typedef long intptr_t;
typedef unsigned long uintptr_t;

// 7.20.1.5 Greatest-width integer types
typedef long intmax_t;
typedef unsigned long uintmax_t;

// -----------------------------------------------------------------------------
// 2. Limits of Specified-Width Integer Types (C11 7.20.2)
// -----------------------------------------------------------------------------

// 7.20.2.1 Limits of exact-width integer types
#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483648)
#define INT64_MIN (-9223372036854775807L - 1)

#define INT8_MAX (127)
#define INT16_MAX (32767)
#define INT32_MAX (2147483647)
#define INT64_MAX (9223372036854775807L)

#define UINT8_MAX (255)
#define UINT16_MAX (65535)
#define UINT32_MAX (4294967295U)
#define UINT64_MAX (18446744073709551615UL)

// 7.20.2.2 Limits of minimum-width integer types
#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

// 7.20.2.3 Limits of fastest minimum-width integer types
#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT64_MIN
#define INT_FAST32_MIN INT64_MIN
#define INT_FAST64_MIN INT64_MIN

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT64_MAX
#define INT_FAST32_MAX INT64_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT64_MAX
#define UINT_FAST32_MAX UINT64_MAX
#define UINT_FAST64_MAX UINT64_MAX

// 7.20.2.4 Limits of integer types capable of holding object pointers
#define INTPTR_MIN INT64_MIN
#define INTPTR_MAX INT64_MAX
#define UINTPTR_MAX UINT64_MAX

// 7.20.2.5 Limits of greatest-width integer types
#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

// -----------------------------------------------------------------------------
// 3. Limits of Other Integer Types (C11 7.20.3)
// -----------------------------------------------------------------------------

// ptrdiff_t limits
#define PTRDIFF_MIN INT64_MIN
#define PTRDIFF_MAX INT64_MAX

// sig_atomic_t limits (usually int on Linux)
#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

// size_t limit
#define SIZE_MAX UINT64_MAX

// wchar_t limits (Linux uses 32-bit signed int for wchar_t)
#define WCHAR_MIN INT32_MIN
#define WCHAR_MAX INT32_MAX

// wint_t limits (Usually 32-bit unsigned int)
#define WINT_MIN 0U
#define WINT_MAX UINT32_MAX

// -----------------------------------------------------------------------------
// 4. Macros for Integer Constants (C11 7.20.4)
// -----------------------------------------------------------------------------

// 7.20.4.1 Macros for minimum-width integer constants
#define INT8_C(x) x
#define INT16_C(x) x
#define INT32_C(x) x
#define INT64_C(x) x##L

#define UINT8_C(x) x
#define UINT16_C(x) x
#define UINT32_C(x) x##U
#define UINT64_C(x) x##UL

// 7.20.4.2 Macros for greatest-width integer constants
#define INTMAX_C(x) x##L
#define UINTMAX_C(x) x##UL

#endif

#ifndef ERROR_H
#define ERROR_H

#include "stdnoreturn.h"

noreturn void error_fatal(char* message);

#endif  // ERROR_H

#ifndef ARENA_H
#define ARENA_H

#include "types.h"

void arena_init(void);
void* arena_alloc(size_t size);
size_t arena_usage_KiB(void);

#define ARENA_ALLOC(Type, count) \
    ((Type*)arena_alloc((count) * sizeof(Type)))

#endif  // ARENA_H

#ifndef STRING_H
#define STRING_H

#include "types.h"

size_t str_len(char* cstr);

#endif  // STRING_H

#ifndef UTILS_H
#define UTILS_H

#define UTILS_MAX(a, b) (((a) > (b)) ? (a) : (b))

#endif  // UTILS_H

#ifndef IOSTREAM_H
#define IOSTREAM_H

#include "linux.h"
#include "types.h"

#define IOSTREAM_STDOUT LINUX_FD_STDOUT
#define IOSTREAM_STDERR LINUX_FD_STDERR

s64 iostream_print_str(s32 stream, char* cstr);
s64 iostream_print_uint(s32 stream, u64 num);

#endif  // IOSTREAM_H

#ifndef FILE_H
#define FILE_H

#include "token.h"
#include "types.h"

typedef struct File {
    // I originate from
    Token* org_tok;

    // this is my name
    char* name;

    // and this is my original data
    u8* og_data;  // raw bytes
    size_t og_size;

    // This is my data after normalizing
    char* norm_data;
    size_t norm_size;

    // This is my data after splicing
    char* spl_data;
    size_t spl_size;

    // This is the line map
    // line_map[logic_line] == phys_line
    size_t* line_map;
    size_t line_map_size;
} File;

File* file_read(char* filename, Token* org_tok);

#endif  //  FILE_H

#ifndef VECTOR_H
#define VECTOR_H

#include "arena.h"
#include "types.h"

// Uses explicit variable names to avoid collion
#define vector_push(vec, elem)                                                                 \
    do {                                                                                       \
        /* 1. Check if we need to grow */                                                      \
        if ((vec)->count >= (vec)->cap) {                                                      \
            /* 2. Calculate explicit sizes */                                                  \
            size_t _current_capacity = (vec)->cap;                                             \
            size_t _new_capacity = (_current_capacity == 0) ? 8 : (_current_capacity * 2);     \
            size_t _element_size = sizeof(*((vec)->data));                                     \
            size_t _allocation_size = _new_capacity * _element_size;                           \
                                                                                               \
            /* 3. Allocate fresh memory from the Arena */                                      \
            void* _new_data = arena_alloc(_allocation_size);                                   \
                                                                                               \
            /* 4. Copy old data manually (if it exists) */                                     \
            if ((vec)->data) {                                                                 \
                u8* _dest_ptr = (u8*)_new_data;                                                \
                u8* _src_ptr = (u8*)((vec)->data);                                             \
                size_t _total_copy_bytes = (vec)->count * _element_size;                       \
                                                                                               \
                for (size_t _byte_index = 0; _byte_index < _total_copy_bytes; _byte_index++) { \
                    _dest_ptr[_byte_index] = _src_ptr[_byte_index];                            \
                }                                                                              \
            }                                                                                  \
                                                                                               \
            /* 5. Update the vector structure */                                               \
            (vec)->data = _new_data;                                                           \
            (vec)->cap = _new_capacity;                                                        \
        }                                                                                      \
                                                                                               \
        /* 6. Push the element */                                                              \
        (vec)->data[(vec)->count++] = (elem);                                                  \
    } while (0)

#endif

#ifndef _STDBOOL_H
#define _STDBOOL_H

// 1. The Type
// C11 uses the built-in keyword '_Bool' for the boolean type.
// This macro makes it accessible as the friendly 'bool'.
#define bool _Bool

// 2. The Values
// must expand to the integer constant 1
#define true 1

// must expand to the integer constant 0
#define false 0

// 3. The Conformance Signal
// This macro signals to the preprocessor that stdbool.h has been included.
// It is required for strict compliance checks.
#define __bool_true_false_are_defined 1

#endif

#ifndef LINUX_H
#define LINUX_H

#include "stdnoreturn.h"
#include "types.h"

#define LINUX_EXIT_FAILURE ((u8)1)
#define LINUX_EXIT_SUCCESS ((u8)0)

#define LINUX_FD_STDOUT ((s32)1)
#define LINUX_FD_STDERR ((s32)2)

#define LINUX_BRK_CURRENT ((u8*)0)

#define LINUX_FILE_MODE_READONLY 0
#define LINUX_FILE_MODE_WRITEONLY 1
#define LINUX_FILE_MODE_READWRITE 2
#define LINUX_FILE_MODE_CREAT 64

#define LINUX_FILE_PERM_USER_RW 438

noreturn void linux_exit(u8 code);
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

#ifndef DIAG_H
#define DIAG_H

#include "stdnoreturn.h"
#include "token.h"

noreturn void diag_error(Token* tok, char* msg);

#endif  // DIAG_H

#ifndef SPLICER_H
#define SPLICER_H

#include "file.h"

void splice(File* file);

#endif  // SPLICER_H

#ifndef NORMALIZER_H
#define NORMALIZER_H

#include "file.h"

void normalize(File* file);

#endif  // NORMALIZER_H

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

#ifndef MAIN_H
#define MAIN_H

#include "types.h"

s32 main(s32 argc, char** argv);

#endif  // MAIN_H


#ifndef TYPES_H
#define TYPES_H

// strict standards
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

// lazy short types for this project
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#endif#

#ifndef _STDNORETURN_H
#define _STDNORETURN_H

#define noreturn _Noreturn

#define __noreturn_is_defined 1

#endif  // _STDNORETURN_H

#ifndef TOKEN_H
#define TOKEN_H

#include "stdnoreturn.h"
#include "types.h"

typedef enum TokenType {
    // --- Internal Control ---
    TOK_EOF,    // End of File
    TOK_ERROR,  // Lexical Error

    // --- C11 Keywords (6.4.1) ---
    TOK_AUTO,           // auto
    TOK_BREAK,          // break
    TOK_CASE,           // case
    TOK_CHAR,           // char
    TOK_CONST,          // const
    TOK_CONTINUE,       // continue
    TOK_DEFAULT,        // default
    TOK_DO,             // do
    TOK_DOUBLE,         // double
    TOK_ELSE,           // else
    TOK_ENUM,           // enum
    TOK_EXTERN,         // extern
    TOK_FLOAT,          // float
    TOK_FOR,            // for
    TOK_GOTO,           // goto
    TOK_IF,             // if
    TOK_INLINE,         // inline
    TOK_INT,            // int
    TOK_LONG,           // long
    TOK_REGISTER,       // register
    TOK_RESTRICT,       // restrict
    TOK_RETURN,         // return
    TOK_SHORT,          // short
    TOK_SIGNED,         // signed
    TOK_SIZEOF,         // sizeof
    TOK_STATIC,         // static
    TOK_STRUCT,         // struct
    TOK_SWITCH,         // switch
    TOK_TYPEDEF,        // typedef
    TOK_UNION,          // union
    TOK_UNSIGNED,       // unsigned
    TOK_VOID,           // void
    TOK_VOLATILE,       // volatile
    TOK_WHILE,          // while
    TOK_ALIGNAS,        // _Alignas
    TOK_ALIGNOF,        // _Alignof
    TOK_ATOMIC,         // _Atomic
    TOK_BOOL,           // _Bool
    TOK_COMPLEX,        // _Complex
    TOK_GENERIC,        // _Generic
    TOK_IMAGINARY,      // _Imaginary
    TOK_NORETURN,       // _Noreturn
    TOK_STATIC_ASSERT,  // _Static_assert
    TOK_THREAD_LOCAL,   // _Thread_local

    // --- Identifiers (6.4.2) ---
    TOK_IDENTIFIER,  // main, x, my_var

    // --- Constants (6.4.4) ---
    TOK_PP_NUMBER,   // 123, 0xFF, 0777, 3.14, 1.2e-10, 0x1.fp3
    TOK_CONST_CHAR,  // 'a', L'a', u'a', U'a'

    // --- String Literals (6.4.5) ---
    TOK_STRING_LITERAL,  // "abc", L"abc", u8"abc", u"abc", U"abc"

    // --- Punctuators (6.4.6) ---
    TOK_LBRACKET,  // [  and  <:
    TOK_RBRACKET,  // ]  and  :>
    TOK_LPAREN,    // (
    TOK_RPAREN,    // )
    TOK_LBRACE,    // {  and  <%
    TOK_RBRACE,    // }  and  %>
    TOK_DOT,       // .
    TOK_ARROW,     // ->

    TOK_INC,        // ++
    TOK_DEC,        // --
    TOK_AMPERSAND,  // &
    TOK_STAR,       // *
    TOK_PLUS,       // +
    TOK_MINUS,      // -
    TOK_TILDE,      // ~
    TOK_BANG,       // !

    TOK_SLASH,        // /
    TOK_PERCENT,      // %
    TOK_LSHIFT,       // <<
    TOK_RSHIFT,       // >>
    TOK_LESS,         // <
    TOK_GREATER,      // >
    TOK_LESS_EQ,      // <=
    TOK_GREATER_EQ,   // >=
    TOK_EQ_EQ,        // ==
    TOK_NOT_EQ,       // !=
    TOK_CARET,        // ^
    TOK_PIPE,         // |
    TOK_LOGICAL_AND,  // &&
    TOK_LOGICAL_OR,   // ||

    TOK_QUESTION,   // ?
    TOK_COLON,      // :
    TOK_SEMICOLON,  // ;
    TOK_ELLIPSIS,   // ...

    TOK_ASSIGN,         // =
    TOK_MUL_ASSIGN,     // *=
    TOK_DIV_ASSIGN,     // /=
    TOK_MOD_ASSIGN,     // %=
    TOK_ADD_ASSIGN,     // +=
    TOK_SUB_ASSIGN,     // -=
    TOK_LSHIFT_ASSIGN,  // <<=
    TOK_RSHIFT_ASSIGN,  // >>=
    TOK_AND_ASSIGN,     // &=
    TOK_XOR_ASSIGN,     // ^=
    TOK_OR_ASSIGN,      // |=

    TOK_COMMA,       // ,
    TOK_HASH,        // #   and  %:
    TOK_DOUBLE_HASH  // ##  and  %:%:

} TokenType;

typedef struct File File;
typedef struct {
    // I live inside
    File* file;

    // inside line
    size_t line;
    char* line_start;

    // at
    size_t col;
    char* text_start;
    size_t text_len;

    // Therefore I am a
    TokenType type;
} Token;

// naming is cruital for use with
// the macro in vector.h
typedef struct {
    Token* data;
    size_t count;
    size_t cap;
} TokenVector;

#endif  // TOKEN_H

#ifndef _STDALIGN_H
#define _STDALIGN_H

// 1. The Alignment Specifier
// Expands to the C11 keyword _Alignas
#define alignas _Alignas

// 2. The Alignment Operator
// Expands to the C11 keyword _Alignof
#define alignof _Alignof

// 3. Conformance Signals
// Required to allow #ifdef checks
#define __alignas_is_defined 1
#define __alignof_is_defined 1

#endif

#ifndef _STDINT_H
#define _STDINT_H

// -----------------------------------------------------------------------------
// 1. Integer Types (C11 7.20.1)
// -----------------------------------------------------------------------------

// 7.20.1.1 Exact-width integer types
// (Required because x86-64 supports 8, 16, 32, and 64-bit twos-complement)
typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

// 7.20.1.2 Minimum-width integer types
typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

// 7.20.1.3 Fastest minimum-width integer types
// On x64, 64-bit integers are technically the native register size.
typedef int8_t int_fast8_t;
typedef int64_t int_fast16_t;
typedef int64_t int_fast32_t;
typedef int64_t int_fast64_t;

typedef uint8_t uint_fast8_t;
typedef uint64_t uint_fast16_t;
typedef uint64_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

// 7.20.1.4 Integer types capable of holding object pointers
typedef long intptr_t;
typedef unsigned long uintptr_t;

// 7.20.1.5 Greatest-width integer types
typedef long intmax_t;
typedef unsigned long uintmax_t;

// -----------------------------------------------------------------------------
// 2. Limits of Specified-Width Integer Types (C11 7.20.2)
// -----------------------------------------------------------------------------

// 7.20.2.1 Limits of exact-width integer types
#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483648)
#define INT64_MIN (-9223372036854775807L - 1)

#define INT8_MAX (127)
#define INT16_MAX (32767)
#define INT32_MAX (2147483647)
#define INT64_MAX (9223372036854775807L)

#define UINT8_MAX (255)
#define UINT16_MAX (65535)
#define UINT32_MAX (4294967295U)
#define UINT64_MAX (18446744073709551615UL)

// 7.20.2.2 Limits of minimum-width integer types
#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

// 7.20.2.3 Limits of fastest minimum-width integer types
#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT64_MIN
#define INT_FAST32_MIN INT64_MIN
#define INT_FAST64_MIN INT64_MIN

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT64_MAX
#define INT_FAST32_MAX INT64_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT64_MAX
#define UINT_FAST32_MAX UINT64_MAX
#define UINT_FAST64_MAX UINT64_MAX

// 7.20.2.4 Limits of integer types capable of holding object pointers
#define INTPTR_MIN INT64_MIN
#define INTPTR_MAX INT64_MAX
#define UINTPTR_MAX UINT64_MAX

// 7.20.2.5 Limits of greatest-width integer types
#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

// -----------------------------------------------------------------------------
// 3. Limits of Other Integer Types (C11 7.20.3)
// -----------------------------------------------------------------------------

// ptrdiff_t limits
#define PTRDIFF_MIN INT64_MIN
#define PTRDIFF_MAX INT64_MAX

// sig_atomic_t limits (usually int on Linux)
#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

// size_t limit
#define SIZE_MAX UINT64_MAX

// wchar_t limits (Linux uses 32-bit signed int for wchar_t)
#define WCHAR_MIN INT32_MIN
#define WCHAR_MAX INT32_MAX

// wint_t limits (Usually 32-bit unsigned int)
#define WINT_MIN 0U
#define WINT_MAX UINT32_MAX

// -----------------------------------------------------------------------------
// 4. Macros for Integer Constants (C11 7.20.4)
// -----------------------------------------------------------------------------

// 7.20.4.1 Macros for minimum-width integer constants
#define INT8_C(x) x
#define INT16_C(x) x
#define INT32_C(x) x
#define INT64_C(x) x##L

#define UINT8_C(x) x
#define UINT16_C(x) x
#define UINT32_C(x) x##U
#define UINT64_C(x) x##UL

// 7.20.4.2 Macros for greatest-width integer constants
#define INTMAX_C(x) x##L
#define UINTMAX_C(x) x##UL

#endif

#ifndef ERROR_H
#define ERROR_H

#include "stdnoreturn.h"

noreturn void error_fatal(char* message);

#endif  // ERROR_H

#ifndef ARENA_H
#define ARENA_H

#include "types.h"

void arena_init(void);
void* arena_alloc(size_t size);

#define ARENA_ALLOC(Type, count) \
    ((Type*)arena_alloc((count) * sizeof(Type)))

#endif  // ARENA_H

#ifndef STRING_H
#define STRING_H

#include "types.h"

size_t str_len(char* cstr);

#endif  // STRING_H

#ifndef UTILS_H
#define UTILS_H

#define UTILS_MAX(a, b) (((a) > (b)) ? (a) : (b))

#endif  // UTILS_H

#ifndef IOSTREAM_H
#define IOSTREAM_H

#include "linux.h"
#include "types.h"

#define IOSTREAM_STDOUT LINUX_FD_STDOUT
#define IOSTREAM_STDERR LINUX_FD_STDERR

s64 iostream_print_str(s32 stream, char* cstr);
s64 iostream_print_uint(s32 stream, u64 num);

#endif  // IOSTREAM_H

#ifndef FILE_H
#define FILE_H

#include "token.h"
#include "types.h"

typedef struct File {
    // I originate from
    Token* org_tok;

    // this is my name
    char* name;

    // and this is my original data
    u8* og_data;  // raw bytes
    size_t og_size;

    // This is currently my data
    // as i evolve
    char* curr_data;
} File;

File* file_read(char* filename, Token* org_tok);

#endif  //  FILE_H

#ifndef VECTOR_H
#define VECTOR_H

#include "arena.h"
#include "types.h"

// Uses explicit variable names to avoid collion
#define vector_push(vec, elem)                                                                 \
    do {                                                                                       \
        /* 1. Check if we need to grow */                                                      \
        if ((vec)->count >= (vec)->cap) {                                                      \
            /* 2. Calculate explicit sizes */                                                  \
            size_t _current_capacity = (vec)->cap;                                             \
            size_t _new_capacity = (_current_capacity == 0) ? 8 : (_current_capacity * 2);     \
            size_t _element_size = sizeof(*((vec)->data));                                     \
            size_t _allocation_size = _new_capacity * _element_size;                           \
                                                                                               \
            /* 3. Allocate fresh memory from the Arena */                                      \
            void* _new_data = arena_alloc(_allocation_size);                                   \
                                                                                               \
            /* 4. Copy old data manually (if it exists) */                                     \
            if ((vec)->data) {                                                                 \
                u8* _dest_ptr = (u8*)_new_data;                                                \
                u8* _src_ptr = (u8*)((vec)->data);                                             \
                size_t _total_copy_bytes = (vec)->count * _element_size;                       \
                                                                                               \
                for (size_t _byte_index = 0; _byte_index < _total_copy_bytes; _byte_index++) { \
                    _dest_ptr[_byte_index] = _src_ptr[_byte_index];                            \
                }                                                                              \
            }                                                                                  \
                                                                                               \
            /* 5. Update the vector structure */                                               \
            (vec)->data = _new_data;                                                           \
            (vec)->cap = _new_capacity;                                                        \
        }                                                                                      \
                                                                                               \
        /* 6. Push the element */                                                              \
        (vec)->data[(vec)->count++] = (elem);                                                  \
    } while (0)

#endif

#ifndef _STDBOOL_H
#define _STDBOOL_H

// 1. The Type
// C11 uses the built-in keyword '_Bool' for the boolean type.
// This macro makes it accessible as the friendly 'bool'.
#define bool _Bool

// 2. The Values
// must expand to the integer constant 1
#define true 1

// must expand to the integer constant 0
#define false 0

// 3. The Conformance Signal
// This macro signals to the preprocessor that stdbool.h has been included.
// It is required for strict compliance checks.
#define __bool_true_false_are_defined 1

#endif

#ifndef LINUX_H
#define LINUX_H

#include "stdnoreturn.h"
#include "types.h"

#define LINUX_EXIT_FAILURE ((u8)1)
#define LINUX_EXIT_SUCCESS ((u8)0)

#define LINUX_FD_STDOUT ((s32)1)
#define LINUX_FD_STDERR ((s32)2)

#define LINUX_BRK_CURRENT ((u8*)0)

#define LINUX_FILE_MODE_READONLY 0
#define LINUX_FILE_MODE_WRITEONLY 1
#define LINUX_FILE_MODE_READWRITE 2
#define LINUX_FILE_MODE_CREAT 64

#define LINUX_FILE_PERM_USER_RW 438

noreturn void linux_exit(u8 code);
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

#ifndef DIAG_H
#define DIAG_H

#include "stdnoreturn.h"
#include "token.h"

noreturn void diag_error(Token* tok, char* msg);

#endif  // DIAG_H

#ifndef NORMALIZER_H
#define NORMALIZER_H

#include "file.h"

char* normalize(File* file);

#endif  // NORMALIZER_H

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

#ifndef MAIN_H
#define MAIN_H

#include "types.h"

s32 main(s32 argc, char** argv);

#endif  // MAIN_H

#include "file.h"

#include "arena.h"
#include "diag.h"
#include "error.h"
#include "linux.h"

noreturn void file_error(Token* org_tok, char* msg) {
    if (org_tok != NULL) {
        diag_error(org_tok, msg);
    } else {
        error_fatal(msg);
    }
    while (true);
}

File* file_read(char* filename, Token* org_tok) {
    s32 fd = linux_open(filename, LINUX_FILE_MODE_READONLY, 0);

    // file open failure
    if (fd < 0) {
        file_error(org_tok, "file not found");
    }

    linux_stat_t stat;
    if (linux_fstat(fd, (linux_stat_t*)(&stat)) < 0) {
        linux_close(fd);
        file_error(org_tok, "failed to stat file");
    }

    size_t size = (size_t)stat.st_size;
    u8* buffer = ARENA_ALLOC(u8, size + 1);

    s64 bytes_read = linux_read(fd, buffer, size);
    if (bytes_read < 0) {
        linux_close(fd);
        file_error(org_tok, "failed to read file");
    }

    if (bytes_read != (s64)size) {
        linux_close(fd);
        file_error(org_tok, "partial read of file");
    }

    linux_close(fd);
    buffer[size] = '\0';

    File* file = ARENA_ALLOC(File, 1);

    file->org_tok = org_tok;
    file->name = filename;
    file->og_data = buffer;
    file->og_size = size;

    // assume we have valid data
    // phase1 will make sure
    file->curr_data = (char*)buffer;

    return file;
}

#include "main.h"

#include "arena.h"
#include "error.h"
#include "file.h"
#include "normalizer.h"
#include "linux.h"

s32 main(s32 argc, char** argv) {
    if (argc < 2) {
        error_fatal("no input file");
    }

    arena_init();

    File* source_file = file_read(argv[1], NULL);

    char* phase1_out = normalize(source_file);

    (void) phase1_out;

    return LINUX_EXIT_SUCCESS;
}

#include "string.h"

size_t str_len(char* cstr) {
    size_t len = 0;
    while (cstr[len]) len++;
    return len;
}

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
    while (true);
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

s64 linux_read(s32 fd, u8* buf, size_t len) {
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

#include "error.h"

#include "iostream.h"
#include "string.h"

noreturn void error_fatal(char* message) {
    char* prefix = "mcc: error: ";

    iostream_print_str(IOSTREAM_STDERR, prefix);
    iostream_print_str(IOSTREAM_STDERR, message);
    iostream_print_str(IOSTREAM_STDERR, "\n");

    linux_exit(LINUX_EXIT_FAILURE);

    // trap to satisfy noreturn
    while (true);
}

#include "arena.h"

#include "error.h"
#include "linux.h"
#include "stdalign.h"
#include "utils.h"

#define ARENA_MiB(x) ((x) << 20)
#define ARENA_SIZE_DEFAULT (ARENA_MiB(1))
#define ARENA_ALIGNMENT_BYTES (alignof(max_align_t))
#define ARENA_ALIGN_UP(number) \
    (((number) + ARENA_ALIGNMENT_BYTES - 1) & ~(ARENA_ALIGNMENT_BYTES - 1))

typedef struct Arena {
    size_t cap;
    size_t pos;
    u8* heap;
} Arena;

// the only global variable
static Arena arena;

static u8* arena_ask_os_for_mem(size_t request) {
    // get current break
    u8* curr_brk = linux_brk(LINUX_BRK_CURRENT);

    size_t req_aligned = ARENA_ALIGN_UP(request);
    u8* next_brk_req = curr_brk + req_aligned;

    // ask for memory
    u8* next_brk = linux_brk(next_brk_req);

    // handle OS refusal
    if (next_brk != next_brk_req) {
        error_fatal("out of memory");
    }

    return curr_brk;
}

void arena_init(void) {
    u8* heap_start = arena_ask_os_for_mem(ARENA_SIZE_DEFAULT);

    arena.cap = ARENA_SIZE_DEFAULT;
    arena.pos = 0;
    arena.heap = heap_start;
}

static void arena_grow(size_t needed_size) {
    size_t req = UTILS_MAX(needed_size, ARENA_SIZE_DEFAULT);
    arena_ask_os_for_mem(req);
    arena.cap += req;
}

void* arena_alloc(size_t size) {
    size_t aligned_size = ARENA_ALIGN_UP(size);

    if (arena.pos + aligned_size > arena.cap) {
        arena_grow(aligned_size);
    }

    u8* ptr = arena.heap + arena.pos;
    arena.pos += aligned_size;

    return (void*)ptr;
}

#include "iostream.h"

#include "string.h"

s64 iostream_print_str(s32 stream, char* cstr) {
    size_t len = str_len(cstr);
    return linux_write(stream, cstr, len);
}

s64 iostream_print_uint(s32 stream, u64 num) {
    size_t pref = num / 10;
    if (pref) {
        iostream_print_uint(stream, pref);
    }

    char c = '0' + (num % 10);
    return linux_write(stream, &c, 1);
}

#include "normalizer.h"

#include "arena.h"
#include "diag.h"

char trigraph_replace(char c) {
    switch (c) {
        case '=':  return '#';
        case '/':  return '\\';
        case '\'': return '^';
        case '(':  return '[';
        case ')':  return ']';
        case '!':  return '|';
        case '<':  return '{';
        case '>':  return '}';
        case '-':  return '~';
        default:   return c;
    }
}

char* normalize(File* file) {
    char* buff = ARENA_ALLOC(char, file->og_size + 1);

    u8* r_ptr = file->og_data;
    char* w_ptr = buff;

    size_t line = 1;
    size_t col = 1;
    char* line_start = buff;

    for (size_t i = 0; i < file->og_size; ++i) {
        // NUL detection
        if (*r_ptr == '\0') {
            diag_error(&(Token){
                           .col = col,
                           .file = file,
                           .line = line,
                           .line_start = line_start,
                           .text_len = 1,
                           .text_start = (char*)r_ptr,  // NUL is a valid char type
                           .type = TOK_ERROR,
                       },
                       "NUL byte in file");
        }

        // trigraph handling
        if (*r_ptr == '?' &&          // this char is '?
            i + 2 < file->og_size &&  // we have at least 2 more chars
            *(r_ptr + 1) == '?'       // the next char is '?'
        ) {
            char replacement = trigraph_replace(*(r_ptr + 2));
            *(w_ptr++) = replacement;
            r_ptr += 3;
        }

        // we have a normal char
        *(w_ptr++) = *(r_ptr++);
    }

    *w_ptr = '\0';
    return buff;
}

#include "diag.h"

#include "file.h"
#include "iostream.h"

void diag_print_snippet(char* line_start) {
    iostream_print_str(IOSTREAM_STDERR, "    | ");

    size_t line_len = 0;
    while (line_start[line_len] && line_start[line_len] != '\n') {
        line_len++;
    }
    linux_write(LINUX_FD_STDERR, line_start, line_len);

    iostream_print_str(IOSTREAM_STDERR, "\n");
}

void diag_print_include_trace(File* file) {
    // base case (root file provided by user)
    if (!file || !file->org_tok) {
        return;
    }

    Token* org = file->org_tok;
    diag_print_include_trace(org->file);

    if (org) {
        iostream_print_str(IOSTREAM_STDERR, "in file included from");
        iostream_print_str(IOSTREAM_STDERR, org->file->name);
        iostream_print_str(IOSTREAM_STDERR, ":");
        iostream_print_uint(IOSTREAM_STDERR, org->line);
        iostream_print_str(IOSTREAM_STDERR, ":\n");
    }
}

void diag_print_header(char* filename, size_t line, size_t col, char* msg) {
    iostream_print_str(IOSTREAM_STDERR, filename);
    iostream_print_str(IOSTREAM_STDERR, ":");
    iostream_print_uint(IOSTREAM_STDERR, line);
    iostream_print_str(IOSTREAM_STDERR, ":");
    iostream_print_uint(IOSTREAM_STDERR, col);
    iostream_print_str(IOSTREAM_STDERR, ": error: ");
    iostream_print_str(IOSTREAM_STDERR, msg);
    iostream_print_str(IOSTREAM_STDERR, "\n");
}

void diag_print_squiggles(size_t col, size_t err_len) {
    for (size_t i = 0; i < col; ++i) {
        iostream_print_str(IOSTREAM_STDERR, " ");
    }

    iostream_print_str(IOSTREAM_STDERR, "^");

    for (size_t i = 0; i < err_len - 1; ++i) {
        iostream_print_str(IOSTREAM_STDERR, "~");
    }

    iostream_print_str(IOSTREAM_STDERR, "\n");
}

noreturn void diag_error(Token* tok, char* msg) {
    diag_print_include_trace(tok->file);

    diag_print_header(tok->file->name, tok->line, tok->col, msg);

    diag_print_snippet(tok->line_start);

    diag_print_squiggles(tok->col, tok->text_len);
    // trap
    while (true);
}

arena memory used = 154 KiB 

#include "file.h"

#include "arena.h"
#include "diag.h"
#include "error.h"
#include "linux.h"

noreturn void file_error(Token* org_tok, char* msg) {
    if (org_tok != NULL) {
        diag_error(org_tok, msg);
    } else {
        error_fatal(msg);
    }
    while (true);
}

File* file_read(char* filename, Token* org_tok) {
    s32 fd = linux_open(filename, LINUX_FILE_MODE_READONLY, 0);

    // file open failure
    if (fd < 0) {
        file_error(org_tok, "file not found");
    }

    linux_stat_t stat;
    if (linux_fstat(fd, (linux_stat_t*)(&stat)) < 0) {
        linux_close(fd);
        file_error(org_tok, "failed to stat file");
    }

    size_t size = (size_t)stat.st_size;
    u8* buffer = ARENA_ALLOC(u8, size + 1);

    s64 bytes_read = linux_read(fd, buffer, size);
    if (bytes_read < 0) {
        linux_close(fd);
        file_error(org_tok, "failed to read file");
    }

    if (bytes_read != (s64)size) {
        linux_close(fd);
        file_error(org_tok, "partial read of file");
    }

    linux_close(fd);
    buffer[size] = '\0';

    File* file = ARENA_ALLOC(File, 1);

    file->org_tok = org_tok;
    file->name = filename;
    file->og_data = buffer;
    file->og_size = size;

    return file;
}

#include "main.h"

#include "arena.h"
#include "error.h"
#include "file.h"
#include "iostream.h"
#include "linux.h"
#include "normalizer.h"
#include "splicer.h"

s32 main(s32 argc, char** argv) {
    if (argc < 2) {
        error_fatal("no input file");
    }

    arena_init();

    File* source_file = file_read(argv[1], NULL);

    normalize(source_file);

    splice(source_file);

    // iostream_print_str(IOSTREAM_STDOUT, source_file->curr_data);

    iostream_print_str(IOSTREAM_STDOUT, "arena memory used = ");
    iostream_print_uint(IOSTREAM_STDOUT, arena_usage_KiB());
    iostream_print_str(IOSTREAM_STDOUT, " KiB \n");
    return LINUX_EXIT_SUCCESS;
}

#include "string.h"

size_t str_len(char* cstr) {
    size_t len = 0;
    while (cstr[len]) len++;
    return len;
}

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
    while (true);
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

s64 linux_read(s32 fd, u8* buf, size_t len) {
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

#include "error.h"

#include "iostream.h"
#include "string.h"

noreturn void error_fatal(char* message) {
    char* prefix = "mcc: error: ";

    iostream_print_str(IOSTREAM_STDERR, prefix);
    iostream_print_str(IOSTREAM_STDERR, message);
    iostream_print_str(IOSTREAM_STDERR, "\n");

    linux_exit(LINUX_EXIT_FAILURE);

    // trap to satisfy noreturn
    while (true);
}

#include "arena.h"

#include "error.h"
#include "linux.h"
#include "stdalign.h"
#include "utils.h"

#define ARENA_KiB(x) ((x) << 10)
#define ARENA_MiB(x) ((x) << 20)
#define ARENA_SIZE_DEFAULT (ARENA_MiB(1))
#define ARENA_ALIGNMENT_BYTES (alignof(max_align_t))
#define ARENA_ALIGN_UP(number) \
    (((number) + ARENA_ALIGNMENT_BYTES - 1) & ~(ARENA_ALIGNMENT_BYTES - 1))

typedef struct Arena {
    size_t cap;
    size_t pos;
    u8* heap;
} Arena;

// the only global variable
static Arena arena;

static u8* arena_ask_os_for_mem(size_t request) {
    // get current break
    u8* curr_brk = linux_brk(LINUX_BRK_CURRENT);

    size_t req_aligned = ARENA_ALIGN_UP(request);
    u8* next_brk_req = curr_brk + req_aligned;

    // ask for memory
    u8* next_brk = linux_brk(next_brk_req);

    // handle OS refusal
    if (next_brk != next_brk_req) {
        error_fatal("out of memory");
    }

    return curr_brk;
}

void arena_init(void) {
    u8* heap_start = arena_ask_os_for_mem(ARENA_SIZE_DEFAULT);

    arena.cap = ARENA_SIZE_DEFAULT;
    arena.pos = 0;
    arena.heap = heap_start;
}

static void arena_grow(size_t needed_size) {
    size_t req = UTILS_MAX(needed_size, ARENA_SIZE_DEFAULT);
    arena_ask_os_for_mem(req);
    arena.cap += req;
}

void* arena_alloc(size_t size) {
    size_t aligned_size = ARENA_ALIGN_UP(size);

    if (arena.pos + aligned_size > arena.cap) {
        arena_grow(aligned_size);
    }

    u8* ptr = arena.heap + arena.pos;
    arena.pos += aligned_size;

    return (void*)ptr;
}

size_t arena_usage_KiB(void) {
    return arena.pos / ARENA_KiB(1);
}

#include "splicer.h"

#include "arena.h"

void splice(File* file) {
    char* in = file->norm_data;
    size_t norm_size = file->norm_size;
    char* out = ARENA_ALLOC(char, norm_size + 1);

    // aggresive buffering (all characters == newlines);
    size_t* line_map = ARENA_ALLOC(size_t, norm_size + 1); // +1 because 1 based indexing
    size_t line_map_size = 0;

    size_t r_idx = 0;
    size_t w_idx = 0;

    size_t phys_line = 1;
    size_t logic_line = 1;

    while (r_idx < norm_size) {
        if (in[r_idx] == '\\' &&      // if this char is a backslash and
            r_idx + 1 < norm_size &&  // we have at least one more char and
            in[r_idx + 1] == '\n'     // the next char is a newline
        ) {
            r_idx += 2;  // skip both
            phys_line++;
        }

        else if (in[r_idx] == '\n') {
            out[w_idx++] = in[r_idx++];
            line_map[logic_line++] = phys_line++;
        }

        else {
            out[w_idx++] = in[r_idx++];
        }
    }

    out[w_idx] = '\0';
    file->spl_data = out;
    file->spl_size = w_idx;
    file->line_map = line_map;
    file->line_map_size = line_map_size;
}

#include "iostream.h"

#include "string.h"

s64 iostream_print_str(s32 stream, char* cstr) {
    size_t len = str_len(cstr);
    return linux_write(stream, cstr, len);
}

s64 iostream_print_uint(s32 stream, u64 num) {
    size_t pref = num / 10;
    if (pref) {
        iostream_print_uint(stream, pref);
    }

    char c = '0' + (num % 10);
    return linux_write(stream, &c, 1);
}

#include "normalizer.h"

#include "arena.h"
#include "diag.h"

char trigraph_replace(char c) {
    switch (c) {
        case '=':  return '#';
        case '/':  return '\\';
        case '\'': return '^';
        case '(':  return '[';
        case ')':  return ']';
        case '!':  return '|';
        case '<':  return '{';
        case '>':  return '}';
        case '-':  return '~';
        default:   return c;
    }
}

void normalize(File* file) {
    u8* in = file->og_data;
    size_t og_size = file->og_size;
    char* out = ARENA_ALLOC(char, og_size + 1);

    size_t line = 1;
    size_t col = 1;
    char* line_start = (char*)in;

    size_t r_idx = 0;
    size_t w_idx = 0;

    while (r_idx < og_size) {
        // NUL detection
        if (in[r_idx] == '\0') {
            diag_error(&(Token){
                           .col = col,
                           .file = file,
                           .line = line,
                           .line_start = line_start,
                           .text_len = 1,
                           .text_start = (char*)(in + r_idx),  // NUL is a valid char type
                           .type = TOK_ERROR,
                       },
                       "NUL byte in file");
        }

        // trigraph handling
        if (in[r_idx] == '?' &&     // this char is '?
            r_idx + 2 < og_size &&  // we have at least 2 more chars
            in[r_idx + 1] == '?'    // the next char is '?'
        ) {
            char replacement = trigraph_replace(in[r_idx + 2]);
            out[w_idx++] = replacement;
            r_idx += 3;
            col += 3;
        }

        // newline handling
        else if (in[r_idx] == '\n') {
            out[w_idx++] = '\n';

            line++;
            col = 1;
            line_start = (char*)(in) + r_idx + 1;
            r_idx++;
        }

        // other characters
        else {
            out[w_idx++] = in[r_idx++];
            col++;
        }
    }

    out[w_idx] = '\0';
    file->norm_data = out;
    file->norm_size = w_idx;
}

#include "diag.h"

#include "file.h"
#include "iostream.h"

void diag_print_include_trace(File* file) {
    // base case (root file provided by user)
    if (!file || !file->org_tok) {
        return;
    }

    Token* org = file->org_tok;
    diag_print_include_trace(org->file);

    if (org) {
        iostream_print_str(IOSTREAM_STDERR, "in file included from");
        iostream_print_str(IOSTREAM_STDERR, org->file->name);
        iostream_print_str(IOSTREAM_STDERR, ":");
        iostream_print_uint(IOSTREAM_STDERR, org->line);
        iostream_print_str(IOSTREAM_STDERR, ":\n");
    }
}

void diag_print_header(char* filename, size_t line, size_t col, char* msg) {
    iostream_print_str(IOSTREAM_STDERR, filename);
    iostream_print_str(IOSTREAM_STDERR, ":");
    iostream_print_uint(IOSTREAM_STDERR, line);
    iostream_print_str(IOSTREAM_STDERR, ":");
    iostream_print_uint(IOSTREAM_STDERR, col);
    iostream_print_str(IOSTREAM_STDERR, ": error: ");
    iostream_print_str(IOSTREAM_STDERR, msg);
    iostream_print_str(IOSTREAM_STDERR, "\n");
}

size_t num_digits(u64 num) {
    size_t count = 1;
    while ((num = num / 10)) count++;
    return count;
}

void diag_print_snippet(size_t line, char* line_start) {
    iostream_print_str(IOSTREAM_STDERR, "   ");
    iostream_print_uint(IOSTREAM_STDERR, line);
    iostream_print_str(IOSTREAM_STDERR, " | ");

    size_t line_len = 0;
    while (line_start[line_len] && line_start[line_len] != '\n') {
        line_len++;
    }
    linux_write(LINUX_FD_STDERR, line_start, line_len);

    iostream_print_str(IOSTREAM_STDERR, "\n");
}

void diag_print_squiggles(size_t line, size_t col, size_t err_len) {
    for (size_t i = 0; i < 3 + num_digits(line) + 1; i++) {
        iostream_print_str(IOSTREAM_STDERR, " ");
    }
    iostream_print_str(IOSTREAM_STDERR, "| ");

    for (size_t i = 0; i < col; ++i) {
        iostream_print_str(IOSTREAM_STDERR, " ");
    }

    iostream_print_str(IOSTREAM_STDERR, "^");

    for (size_t i = 0; i < err_len - 1; ++i) {
        iostream_print_str(IOSTREAM_STDERR, "~");
    }

    iostream_print_str(IOSTREAM_STDERR, "\n");
}

noreturn void diag_error(Token* tok, char* msg) {
    diag_print_include_trace(tok->file);

    diag_print_header(tok->file->name, tok->line, tok->col, msg);

    diag_print_snippet(tok->line, tok->line_start);

    diag_print_squiggles(tok->line, tok->col, tok->text_len);

    linux_exit(LINUX_EXIT_FAILURE);
}
