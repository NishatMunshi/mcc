#include <arena.h>
#include <expander.h>
#include <panic.h>
#include <vector.h>

#define LIBS_DIR "./include/"

static bool is_at_start_of_line(PPTokenVector pptokens, size_t i) {
    s64 index = i;

    // 3. We want to verify there is nothing BUT whitespace before the hash.
    index--;
    while (index >= 0 && pptokens.data[index].kind == PP_WHITESPACE) index--;

    // 4. Now check the boundary
    if (index < 0) return true;                                 // Start of file (Valid)
    if (pptokens.data[index].kind != PP_NEWLINE) return false;  // Found junk before hash (Invalid)

    return true;
}

static char* extract_relative_path(PPToken pptoken) {
    char* buf = ARENA_ALLOC(char, pptoken.length - 1);

    for (size_t i = 1; i < pptoken.length - 1; ++i) {
        buf[i - 1] = pptoken.splicedchar_start[i].value;
    }

    return buf;
}

char* get_directory(char* full_path) {
    char* last_slash = strrchr(full_path, '/');

    if (!last_slash) {
        return strdup("./");
    }

    size_t len = last_slash - full_path + 1;
    char* buff = ARENA_ALLOC(char, len + 1);
    memcpy(buff, full_path, len);
    buff[len] = '\0';

    return buff;
}

static ExpandedTokenVector handle_include(PPTokenVector pptokens, size_t i) {
    i++;  // skip the "include"
    while (pptokens.data[i].kind == PP_WHITESPACE) i++;

    char* full_path = nullptr;
    if (pptokens.data[i].kind == PP_HEADERNAME) {
        char* relative_path = extract_relative_path(pptokens.data[i]);
        full_path = strcat(LIBS_DIR, relative_path);
    }

    else if (pptokens.data[i].kind == PP_STRING) {
        if (pptokens.data[i].splicedchar_start->value != '\"') panic("encoded strings not allowed after \"#include\"");
        char* this_file_dir = get_directory(pptokens.data[i].splicedchar_start->source_char->byte->inclusion->definition->full_path);
        char* relative_path = extract_relative_path(pptokens.data[i]);
        full_path = strcat(this_file_dir, relative_path);
    }

    else {
        panic("expected file full_path after \"#include\"");
    }

    ByteVector bytes = read(full_path, pptokens.data + i);
    SourceCharVector source_chars = normalize(bytes);
    SplicedCharVector spliced_chars = splice(source_chars);
    PPTokenVector pptokens_new = tokenize(spliced_chars);
    return expand(pptokens_new);
}

static size_t skip_to_next_line(PPTokenVector pptokens, size_t i) {
    while (pptokens.data[i].kind != PP_NEWLINE) i++;
    return i + 1;
}

ExpandedTokenVector expand(PPTokenVector pptokens) {
    ExpandedTokenVector expanded_tokens = {0};

    for (size_t i = 0; i < pptokens.count;) {
        if (
            pptoken_is(pptokens.data[i], PP_PUNCTUATOR, "#") &&
            is_at_start_of_line(pptokens, i)
        ) {
            i++;
            while (pptokens.data[i].kind == PP_WHITESPACE) i++;

            if (pptoken_is(pptokens.data[i], PP_IDENTIFIER, "include")) {
                ExpandedTokenVector included_tokens = handle_include(pptokens, i);
                vector_append(&expanded_tokens, included_tokens);
                i = skip_to_next_line(pptokens, i);
            }

            else {
                i = skip_to_next_line(pptokens, i);
            }
        }

        else {
            ExpandedToken expanded_token = {
                .kind = pptokens.data[i].kind,
                .spelling = strdup(pptokens.data[i].spelling),
                .length = pptokens.data[i].length,

                .pptoken = pptokens.data + i,
            };

            vector_push(&expanded_tokens, expanded_token);
            i++;
        }
    }

    return expanded_tokens;
}
