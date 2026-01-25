#include <arena.h>
#include <panic.h>
#include <splicer.h>
#include <vector.h>

SplicedCharVector* splice(SourceCharVector* source_chars) {
    // check newline at end of file
    if (source_chars->count >= 1 &&
        source_chars->data[source_chars->count - 1]->value != '\n') {
        panic_sourcechar(source_chars->data[source_chars->count - 1],
                         "no newline at end of file");
    }

    /*
    Now we know last character IS newline.
    Check backslash just before said newline.
    */
    if (source_chars->count >= 2 &&
        source_chars->data[source_chars->count - 2]->value == '\\') {
        panic_sourcechar(source_chars->data[source_chars->count - 2],
                         "backslash before last newline");
    }

    SplicedCharVector* spliced_chars = ARENA_ALLOC(SplicedCharVector, 1);

    for (size_t i = 0; i < source_chars->count;) {
        /*
        We don't need the bounds check for lookahead
        because the '\\' is not the last character,
        guaranteed by the checks above.
        */
        if (source_chars->data[i]->value == '\\' &&
            source_chars->data[i + 1]->value == '\n') {
            i += 2;
            continue;
        }

        SplicedChar* spliced_char = ARENA_ALLOC(SplicedChar, 1);
        spliced_char->value = source_chars->data[i]->value;
        spliced_char->source_char = source_chars->data[i];

        vector_push(spliced_chars, spliced_char);

        i += 1;
    }

    return spliced_chars;
}
