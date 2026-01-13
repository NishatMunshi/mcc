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
        iostream_print_uint(IOSTREAM_STDERR, org->logic_line);
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

void diag_print_snippet(File* file, size_t logic_line) {
    size_t idx = 0;
    size_t curr_logic_line = 1;
    while (idx < file->spl_size) {
        if (curr_logic_line == logic_line) {
            break;
        }

        if (file->spl_data[idx] == '\n') {
            curr_logic_line++;
        }
        idx++;
    }

    char* line_start = file->spl_data + idx;

    iostream_print_str(IOSTREAM_STDERR, "   ");
    iostream_print_uint(IOSTREAM_STDERR, logic_line);
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

    for (size_t i = 0; i < col - 1; ++i) {
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

    diag_print_header(tok->file->name, tok->logic_line, tok->logic_col, msg);

    diag_print_snippet(tok->file, tok->logic_line);

    diag_print_squiggles(tok->logic_line, tok->logic_col, tok->text_len);

    linux_exit(LINUX_EXIT_FAILURE);
}
