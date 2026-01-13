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
