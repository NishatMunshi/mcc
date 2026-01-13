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

    size_t line = 1;
    size_t col = 1;
    char* line_start = (char*)file->og_data;

    size_t r_idx = 0;
    size_t w_idx = 0;

    while (r_idx < file->og_size) {
        // NUL detection
        if (file->og_data[r_idx] == '\0') {
            diag_error(&(Token){
                           .col = col,
                           .file = file,
                           .line = line,
                           .line_start = line_start,
                           .text_len = 1,
                           .text_start = (char*)(file->og_data + r_idx),  // NUL is a valid char type
                           .type = TOK_ERROR,
                       },
                       "NUL byte in file");
        }

        // trigraph handling
        if (file->og_data[r_idx] == '?' &&   // this char is '?
            r_idx + 2 < file->og_size &&     // we have at least 2 more chars
            file->og_data[r_idx + 1] == '?'  // the next char is '?'
        ) {
            char replacement = trigraph_replace(file->og_data[r_idx + 2]);
            buff[w_idx++] = replacement;
            r_idx += 3;
            col += 3;
        }

        // newline handling
        else if(file->og_data[r_idx] == '\n') {
            buff[w_idx++] = '\n';

            line++;
            col = 1;
            line_start = (char*)(file->og_data) + r_idx + 1;
            r_idx++;
        }

        // other characters
        else {
            buff[w_idx++] = file->og_data[r_idx++];
            col++;
        }
    }

    buff[w_idx] = '\0';
    return buff;
}
