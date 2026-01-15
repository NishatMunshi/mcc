#include <expander.h>

#include <arena.h>
#include <panic.h>
#include <vector.h>
#include <io.h>

#define LIBS_DIR "./include/"

static bool pptoken_is(PPToken pptoken, PPTokenKind kind, char* spelling) {
    if (pptoken.kind != kind) return false;

    size_t spelling_len = strlen(spelling);
    if (pptoken.length != spelling_len) return false;

    for (size_t i = 0; i < spelling_len; ++i) {
        if (pptoken.splicedchar_start[i].value != spelling[i]) return false;
    }

    return true;
}

static bool is_at_start_of_line(PPTokenVector* pptokens, size_t i) {
    s64 index = i;

    // 3. We want to verify there is nothing BUT whitespace before the hash.
    index--;
    while (index >= 0 && pptokens->data[index].kind == PP_WHITESPACE) index--;

    // 4. Now check the boundary
    if (index < 0) return true;                                  // Start of file (Valid)
    if (pptokens->data[index].kind != PP_NEWLINE) return false;  // Found junk before hash (Invalid)

    return true;
}

static char* extract_filename(PPToken pptoken) {
    char* buf = ARENA_ALLOC(char, pptoken.length - 1);

    for (size_t i = 1; i < pptoken.length - 1; ++i) {
        buf[i - 1] = pptoken.splicedchar_start[i].value;
    }

    return buf;
}

char* get_directory(char* full_path) {
    char* last_slash = strrchr(full_path, '/');

    if(!last_slash) {
        return strdup("./");
    }

    size_t len = last_slash - full_path + 1;
    char* buff = ARENA_ALLOC(char, len + 1);
    memcpy(buff, full_path, len);
    buff[len] = '\0';

    return buff;
}

static ExpandedTokenVector* handle_include(PPTokenVector* pptokens, size_t i) {
    i++;  // skip the "include"
    while (pptokens->data[i].kind == PP_WHITESPACE) i++;

    char* path = nullptr;
    if (pptokens->data[i].kind == PP_HEADERNAME) {
        char* filename_to_include = extract_filename(pptokens->data[i]);
        path = strcat(LIBS_DIR, filename_to_include);
    }
    
    else if (pptokens->data[i].kind == PP_STRING) {
        if (pptokens->data[i].splicedchar_start->value != '\"') panic("encoded strings not allowed after \"#include\"");
        char* this_file_dir = get_directory(pptokens->data[i].splicedchar_start->source_char->byte->inclusion->definition->path);
        char* filename_to_include = extract_filename(pptokens->data[i]);
        path = strcat(this_file_dir, filename_to_include);
    }

    else {
        panic("expected file path after \"#include\"");
    }

    ByteVector* bytes = read(path, pptokens->data + i);
    SourceCharVector* source_chars = normalize(bytes);
    SplicedCharVector* spliced_chars = splice(source_chars);
    PPTokenVector* pptokens_new = tokenize(spliced_chars);
    return expand(pptokens_new);
}

static size_t skip_to_next_line(PPTokenVector* pptokens, size_t i) {
    while (pptokens->data[i].kind != PP_NEWLINE) i++;
    return i + 1;
}

ExpandedTokenVector* expand(PPTokenVector* pptokens) {
    ExpandedTokenVector* expanded_tokens = ARENA_ALLOC(ExpandedTokenVector, 1);

    for (size_t i = 0; i < pptokens->count;) {
        if (
            pptoken_is(pptokens->data[i], PP_PUNCTUATOR, "#") &&
            is_at_start_of_line(pptokens, i)
        ) {
            i++;
            while (pptokens->data[i].kind == PP_WHITESPACE) i++;

            if (pptoken_is(pptokens->data[i], PP_IDENTIFIER, "include")) {
                ExpandedTokenVector* included_tokens = handle_include(pptokens, i);
                vector_append(expanded_tokens, included_tokens);
                i = skip_to_next_line(pptokens, i);
            }

            else {
                i = skip_to_next_line(pptokens, i);
            }
        }

        else {
            ExpandedToken expanded_token = {
                .pptoken = pptokens->data + i,
            };

            vector_push(expanded_tokens, expanded_token);
            i++;
        }
    }

    return expanded_tokens;
}
