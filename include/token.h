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
    size_t line_len;

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
