#include <splicer.h>

#include <arena.h>
#include <panic.h>
#include <vector.h>

SplicedCharVector* splice(SourceCharVector* source_chars) {
    SplicedCharVector* spliced_chars = ARENA_ALLOC(SplicedCharVector, 1);

    for (size_t i = 0; i < source_chars->count;) {
        char left = source_chars->data[i].value;

        if (i + 1 == source_chars->count && left != '\n') panic("no newline at end of file");

        if (left == '\\' && i + 2 == source_chars->count && source_chars->data[i + 1].value == '\n') panic("backslash before last newline");

        if (left == '\\' && i + 1 < source_chars->count && source_chars->data[i + 1].value == '\n') {
            i += 2;
            continue;
        }

        SplicedChar spliced_char = {
            .source_char = source_chars->data + i,
            .value = left,
        };

        i++;

        vector_push(spliced_chars, spliced_char);
    }

    return spliced_chars;
}
