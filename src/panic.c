#include <expander.h>
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
        if (i == byte->offset) {
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

void print_caret(size_t line, size_t col) {
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

void print_include_trace(FileInclusion* inclusion) {
    if (inclusion->inclusion_trigger == nullptr) {
        return;
    }

    PPToken* header_name_token = inclusion->inclusion_trigger;
    SplicedChar* header_name_first_splicedchar = header_name_token->origin->data[0];
    SourceChar* header_name_first_sourcechar = header_name_first_splicedchar->source_char;
    Byte* header_name_first_byte = header_name_first_sourcechar->origin->data[0];
    FileInclusion* parent_inclusion = header_name_first_byte->origin;

    print_include_trace(parent_inclusion);

    Location include_loc = byte_get_location(header_name_first_byte);

    eprintf("In file included from %s:%zu:\n", include_loc.filename, include_loc.line);
}
