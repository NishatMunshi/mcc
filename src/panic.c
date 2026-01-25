#include <io.h>
#include <linux.h>
#include <panic.h>

Location byte_get_location(Byte* byte) {
    Location loc = {
        .filename = byte->origin->definition->full_path,
        .line = 1,
        .col = 1,
    };

    FileDefinition* def = byte->origin->definition;
    for (size_t i = 0; i < def->size; ++i) {
        if (def->content[i] == byte->value) {
            break;
        }

        if (def->content[i] == '\n') {
            loc.line++;
            loc.col = 1;
        }

        else {
            loc.col++;
        }
    }

    return loc;
}

size_t num_digits(size_t num) {
    if (num == 0) {
        return 1;
    }

    size_t num_digits = 0;
    while (true) {
        num_digits++;

        size_t prefix = num / 10;

        if (prefix == 0) {
            break;
        }

        num = prefix;
    }

    return num_digits;
}

void print_line(u8* line_start) {
    size_t line_len = 0;
    while (true) {
        if (line_start[line_len] == '\0') {
            break;
        }

        if (line_start[line_len] == '\n') {
            break;
        }

        line_len++;
    }
    linux_write(LINUX_FD_STDERR, (char*)line_start, line_len);
}

void print_snippet(FileDefinition* def, size_t line) {
    eprintf("  %zu | ", line);

    size_t current_line = 1;
    for (size_t i = 0; i < def->size; ++i) {
        if (current_line == line) {
            print_line(def->content + i);
            eprintf("\n");
            break;
        }

        else if (def->content[i] == '\n') {
            current_line++;
            continue;
        }

        else {
            continue;
        }
    }
}

void print_squiggles(size_t line, size_t col) {
    eprintf("  ");

    for (size_t i = 0; i < num_digits(line); ++i) {
        eprintf(" ");
    }

    eprintf(" |");

    for (size_t i = 0; i < col; ++i) {
        eprintf(" ");
    }

    eprintf("^\n");
}
