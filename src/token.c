#include "token.h"

#include "arena.h"
#include "file.h"
#include "iostream.h"

void token_print_type(TokenType type) {
    switch (type) {
        case TOK_NEWLINE:    iostream_print_str(IOSTREAM_STDOUT, "TOK_NEWLINE"); break;
        case TOK_WHITESPACE: iostream_print_str(IOSTREAM_STDOUT, "TOK_WHITESPACE"); break;
        default:             iostream_print_str(IOSTREAM_STDOUT, "TOK_ERROR"); break;
    }
}

void token_print(Token* tok) {
    iostream_print_str(IOSTREAM_STDOUT, tok->file->name);
    iostream_print_str(IOSTREAM_STDOUT, ":");
    iostream_print_uint(IOSTREAM_STDOUT, tok->logic_line);
    iostream_print_str(IOSTREAM_STDOUT, ":");
    iostream_print_uint(IOSTREAM_STDOUT, tok->logic_col);
    iostream_print_str(IOSTREAM_STDOUT, ": <TYPE = ");
    token_print_type(tok->type);
    iostream_print_str(IOSTREAM_STDOUT, ">: ");

    iostream_print_str(IOSTREAM_STDOUT, "||");
    linux_write(LINUX_FD_STDOUT, tok->text_start, tok->text_len);
    iostream_print_str(IOSTREAM_STDOUT, "||\n");
}
