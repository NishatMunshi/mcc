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

    size_t r_idx = 0;
    size_t w_idx = 0;

    while (r_idx < og_size) {
        // NUL detection
        if (in[r_idx] == '\0') {
            diag_error(&(Token){
                           .logic_col = col,
                           .file = file,
                           .logic_line = line,
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

        /*
        Physical source file multibyte characters are mapped,
        in an implementation- defined manner,
        to the source character set
        (introducing new-line characters for end-of-line indicators) if necessary.
        */
        /*
        This is handled in x64 linux butomatically
        i.e. the mapping is an idenitity function
        */

        // newline handling
        else if (in[r_idx] == '\n') {
            out[w_idx++] = '\n';

            line++;
            col = 1;
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
