#include "splicer.h"

#include "arena.h"
#include "diag.h"

void splice(File* file) {
    char* in = file->norm_data;
    size_t norm_size = file->norm_size;
    char* out = ARENA_ALLOC(char, norm_size + 1);

    // aggresive buffering (all characters == newlines);
    size_t* line_map = ARENA_ALLOC(size_t, norm_size + 1);  // +1 because 1 based indexing
    size_t line_map_size = 0;

    size_t r_idx = 0;
    size_t w_idx = 0;

    size_t phys_line = 1;
    size_t phys_col = 1;
    size_t logic_line = 1;

    while (r_idx < norm_size) {
        /*
        A source file that is not empty shall end in a new-line character
        */
        if (r_idx + 1 == norm_size && in[r_idx] != '\n') {
            diag_error(&(Token){
                           .file = file,
                           .logic_col = phys_col,
                           .logic_line = phys_line,
                           .text_len = 1,
                           .text_start = in + r_idx,
                           .type = TOK_ERROR,
                       },
                       "no newline at end of file");
        }

        /*
        which shall not be immediately preceded by a backslash character before any such splicing takes place.
        */
        if (r_idx + 2 == norm_size && in[r_idx] == '\\' && in[r_idx + 1] == '\n') {
            diag_error(&(Token){
                           .file = file,
                           .logic_col = phys_col,
                           .logic_line = phys_line,
                           .text_len = 1,
                           .text_start = in + r_idx,
                           .type = TOK_ERROR,
                       },
                       "'\\' immediately before newline");
        }

        /*
        Each instance of a backslash character (\) immediately followed by a new-line character is deleted,
        splicing physical source lines to form logical source lines.
        Only the last backslash on any physical source line shall be eligible for being part of such a splice
        */
        if (in[r_idx] == '\\' &&      // if this char is a backslash and
            r_idx + 1 < norm_size &&  // we have at least one more char and
            in[r_idx + 1] == '\n'     // the next char is a newline
        ) {
            r_idx += 2;  // skip both
            phys_line++;
            phys_col = 1;
        }

        else if (in[r_idx] == '\n') {
            out[w_idx++] = in[r_idx++];
            line_map[logic_line++] = phys_line++;
            phys_col = 1;
        }

        else {
            out[w_idx++] = in[r_idx++];
            phys_col++;
        }
    }

    out[w_idx] = '\0';
    file->spl_data = out;
    file->spl_size = w_idx;
    file->line_map = line_map;
    file->line_map_size = line_map_size;
}
