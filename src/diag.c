#include "diag.h"

#include "file.h"
#include "iostream.h"

void diag_print_snippet(char* line_start, size_t line_len) {
    iostream_print_str(IOSTREAM_STDERR, "    | ");
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

    diag_print_snippet(tok->line_start, tok->line_len);

    diag_print_squiggles(tok->col, tok->text_len);
    // trap
    while (true);
}
