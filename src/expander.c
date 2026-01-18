#include <arena.h>
#include <expander.h>
#include <io.h>
#include <panic.h>
#include <vector.h>

#define LIBS_DIR "./include/"
#define MAX_INCLUDE_DEPTH 15

typedef struct MacroDefinitionMap {
    // double pointer indirection is needed because this
    // data is mutable
    // (used at the same time it is being updated)
    // This problem only happens because expand()
    // is a recursive function
    MacroDefinition** data;

    size_t count;
    size_t capacity;
} MacroDefinitionMap;

static MacroDefinitionMap g_macro_definitions = {0};
static size_t include_depth = 0;

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

static char* get_directory(char* full_path) {
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

static size_t skip_whitespace(PPTokenVector pptokens, size_t i) {
    while (pptokens.data[i].kind == PP_WHITESPACE) i++;
    return i;
}

static ExpandedTokenVector handle_include(PPTokenVector pptokens, size_t i) {
    if (include_depth > MAX_INCLUDE_DEPTH) panic("maximum include depth reached");

    i++;  // skip the "include"
    i = skip_whitespace(pptokens, i);

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

    include_depth++;
    ByteVector* bytes = read(full_path, pptokens.data + i);
    SourceCharVector* source_chars = normalize(bytes);
    SplicedCharVector* spliced_chars = splice(source_chars);
    PPTokenVector pptokens_new = tokenize(*spliced_chars);
    ExpandedTokenVector expanded_tokens_new = expand(pptokens_new);
    include_depth--;

    return expanded_tokens_new;
}

static size_t skip_to_next_line(PPTokenVector pptokens, size_t i) {
    while (pptokens.data[i].kind != PP_NEWLINE) i++;
    return i;
}

static MacroDefinition* find_macro_definition(char* name) {
    for (size_t i = 0; i < g_macro_definitions.count; ++i) {
        if (g_macro_definitions.data[i]->is_undefined) continue;
        if (streq(g_macro_definitions.data[i]->name, name)) return g_macro_definitions.data[i];
    }
    return nullptr;
}

static bool pptokenvector_contains(PPTokenVector pptokens, char* spelling) {
    for (size_t i = 0; i < pptokens.count; ++i) {
        if (streq(pptokens.data[i].spelling, spelling)) return true;
    }
    return false;
}

// Consumes pptokens inside the parentheses of a macro definition.
// Returns: [a, b]
// Updates: is_variadic = true, index points AFTER the closing ')'
PPTokenVector parse_macro_params(PPTokenVector pptokens, size_t* params_end_index, bool* is_variadic) {
    PPTokenVector params = {0};
    *is_variadic = false;
    size_t i = *params_end_index;

    // eat the '('
    i++;

    // 1. Handle Empty Case: "()"
    // We are currently pointing just after the opening '('.
    i = skip_whitespace(pptokens, i);
    if (pptoken_is(pptokens.data[i], PP_PUNCTUATOR, ")")) {
        *params_end_index = i + 1;  // Return index pointing after ')'
        return params;
    }

    // 2. Loop through parameters
    while (true) {
        i = skip_whitespace(pptokens, i);

        // Case A: Variadic Ellipsis "..."
        if (pptoken_is(pptokens.data[i], PP_PUNCTUATOR, "...")) {
            *is_variadic = true;
            i++;

            // "..." must be the strictly last parameter.
            // We expect a closing ')' immediately (ignoring whitespace).
            i = skip_whitespace(pptokens, i);
            if (!pptoken_is(pptokens.data[i], PP_PUNCTUATOR, ")")) {
                panic("Expected ')' after '...' in macro parameter list");
            }

            i++;    // Eat the ')'
            break;  // We are done
        }

        // Case B: Regular Identifier
        else if (pptokens.data[i].kind == PP_IDENTIFIER) {
            // Store the parameter name
            if (pptokenvector_contains(params, pptokens.data[i].spelling)) panic("duplicate param name");

            vector_push(&params, pptokens.data[i]);
            i++;

            // Check what follows: ',' or ')'
            i = skip_whitespace(pptokens, i);

            if (pptoken_is(pptokens.data[i], PP_PUNCTUATOR, ",")) {
                i++;  // Eat comma, loop back for next param
                continue;
            } else if (pptoken_is(pptokens.data[i], PP_PUNCTUATOR, ")")) {
                i++;  // Eat ')', we are done
                break;
            } else {
                panic("Expected ',' or ')' after macro parameter");
            }
        }

        // Case C: Syntax Error (e.g. "int a" or numbers)
        else {
            panic("Expected identifier or '...' in macro definition");
        }
    }

    // Update the caller's index to point to the start of the replacement list
    *params_end_index = i;
    return params;
}

PPTokenVector parse_macro_replacement(PPTokenVector pptokens, size_t i) {
    PPTokenVector replacement_list = {0};
    // splicer guarantees newline at end of file
    // so this loop is guaranteed to terminate
    while (pptokens.data[i].kind != PP_NEWLINE) {
        vector_push(&replacement_list, pptokens.data[i]);
        i++;
    }
    return replacement_list;
}

static void handle_define(PPTokenVector pptokens, size_t i) {
    // skip the "define" and any whitespace after
    i++;
    i = skip_whitespace(pptokens, i);

    if (pptokens.data[i].kind != PP_IDENTIFIER) panic("expected identifier after \"#define\"");
    if (find_macro_definition(pptokens.data[i].spelling)) {
        printf("redefinition of macro %s\n", pptokens.data[i].spelling);
    }

    MacroDefinition* definition = ARENA_ALLOC(MacroDefinition, 1);
    definition->name = strdup(pptokens.data[i].spelling);
    i++;

    if (pptoken_is(pptokens.data[i], PP_PUNCTUATOR, "(")) {
        definition->is_function_like = true;

        // parse up to and including the closing paren
        size_t params_end_index = i;
        PPTokenVector params = parse_macro_params(pptokens, &params_end_index, &definition->is_variadic);
        definition->params = params;
        i = params_end_index;
    }

    else {
        definition->is_function_like = false;
    }

    i = skip_whitespace(pptokens, i);

    PPTokenVector replacement_list = parse_macro_replacement(pptokens, i);
    definition->replacement_list = replacement_list;
    definition->is_expanding = false;
    definition->is_undefined = false;

    vector_push(&g_macro_definitions, definition);
}

static ExpandedToken passthrough(PPToken* pptoken) {
    ExpandedToken expanded_token = {
        .kind = pptoken->kind,
        .spelling = strdup(pptoken->spelling),
        .length = pptoken->length,

        .pptoken = pptoken,

        // this is normal code, not a macro invocation
        .invocation = nullptr,
    };

    return expanded_token;
}

// Returns -1 if not found, or 0..N if found
static ssize_t get_param_index(MacroDefinition* def, char* name) {
    for (size_t i = 0; i < def->params.count; i++) {
        if (streq(def->params.data[i].spelling, name)) return i;
    }
    return -1;
}

static ExpandedTokenVector expand_object_like_macro(MacroDefinition* definition, PPToken* source_token) {
    MacroInvocation* invoc = ARENA_ALLOC(MacroInvocation, 1);
    invoc->definition = definition;
    invoc->pptoken = source_token;  // The token that triggered this
    invoc->args = (PPTokenVectorVector){0};

    definition->is_expanding = true;

    // ... expand ...
    ExpandedTokenVector final = expand(definition->replacement_list);

    // ... fix traceability pointers ...
    for (size_t k = 0; k < final.count; k++) {
        if (!final.data[k].invocation) final.data[k].invocation = invoc;
    }

    definition->is_expanding = false;
    return final;
}

static size_t skip_whitespace_and_newlines(PPTokenVector pptokens, size_t i) {
    while (pptokens.data[i].kind == PP_WHITESPACE || pptokens.data[i].kind == PP_NEWLINE) i++;
    return i;
}

PPTokenVectorVector parse_macro_invocation_args(PPTokenVector pptokens, size_t* index) {
    PPTokenVectorVector args = {0};
    size_t i = *index;

    // 1. Move past the Macro Name
    i++;

    // 2. Find the opening '('
    // Arguments can start on the next line: #define A(x) ... \n A \n (10)
    i = skip_whitespace_and_newlines(pptokens, i);

    // Sanity check (Caller should have guaranteed this)
    if (i >= pptokens.count || !pptoken_is(pptokens.data[i], PP_PUNCTUATOR, "(")) {
        panic("Expected '(' after function-like macro invocation");
    }

    // Eat the '('
    i++;

    // 3. Argument Parsing Loop
    PPTokenVector current_arg = {0};
    int paren_depth = 0;

    // Handle edge case: FOO() -> 1 argument (empty) OR 0 arguments?
    // We check for immediate ')' before entering the main token accumulation.
    // However, the cleanest way is to enter the loop. If we hit ')' immediately,
    // we push one empty vector. The expander logic handles "0 vs 1" param checks.

    while (i < pptokens.count) {
        PPToken pptoken = pptokens.data[i];

        // Case A: Nested Parentheses
        if (pptoken_is(pptoken, PP_PUNCTUATOR, "(")) {
            paren_depth++;
            vector_push(&current_arg, pptoken);
            i++;
            continue;
        }

        // Case B: Closing Parenthesis
        if (pptoken_is(pptoken, PP_PUNCTUATOR, ")")) {
            if (paren_depth == 0) {
                // End of Invocation!

                // Push the final argument collected so far.
                // Note: For "FOO()", current_arg is empty, which is correct.
                vector_push(&args, current_arg);

                i++;  // Eat the closing ')'
                break;
            } else {
                // Just closing a nested group
                paren_depth--;
                vector_push(&current_arg, pptoken);
                i++;
                continue;
            }
        }

        // Case C: Comma Separator
        if (pptoken_is(pptoken, PP_PUNCTUATOR, ",")) {
            if (paren_depth == 0) {
                // This marks the end of an argument.
                vector_push(&args, current_arg);

                // Reset for the next argument
                // We zero it out so the next vector_push starts a fresh buffer
                memset(&current_arg, 0, sizeof(current_arg));

                i++;  // Eat the comma
                continue;
            }
            // Else: Comma inside nested parens (e.g. function call in arg), just treat as text.
        }

        // Case D: Newlines
        // C Standard 6.10.3.11: "New-line characters in the arguments are each
        // replaced by a space character."
        if (pptoken.kind == PP_NEWLINE) {
            PPToken space_token = pptoken;
            space_token.kind = PP_WHITESPACE;
            space_token.spelling = " ";
            space_token.length = 1;
            vector_push(&current_arg, space_token);
            i++;
            continue;
        }

        // Case E: Normal Token
        vector_push(&current_arg, pptoken);
        i++;
    }

    if (paren_depth > 0) {
        panic("Unterminated macro invocation: missing ')'");
    }

    // Update caller's index to point AFTER the invocation
    *index = i;
    return args;
}

static ExpandedTokenVector expand_function_like_macro(MacroDefinition* definition, PPTokenVector pptokens, size_t* i) {
    // 1. Create Invocation Object (For traceability)
    MacroInvocation* invoc = ARENA_ALLOC(MacroInvocation, 1);
    invoc->definition = definition;
    invoc->pptoken = &pptokens.data[*i];  // Points to the name before we move i

    // 2. Parse Arguments
    // parse_args must handle the '(' and ')' and commas
    PPTokenVectorVector args = parse_macro_invocation_args(pptokens, i);
    invoc->args = args;

    // Error checks...
    if (args.count < definition->params.count) panic("Too few args");
    if (!definition->is_variadic && args.count > definition->params.count) panic("Too many args");

    // 3. Lock
    definition->is_expanding = true;

    // 4. Substitution (The "Paste")
    PPTokenVector pasted_tokens = {0};

    for (size_t j = 0; j < definition->replacement_list.count; ++j) {
        PPToken pptoken = definition->replacement_list.data[j];

        if (pptoken.kind == PP_IDENTIFIER) {
            // Find WHICH parameter this is (e.g., param index 0?)
            ssize_t param_idx = get_param_index(definition, pptoken.spelling);

            if (param_idx != -1) {
                // It IS a parameter!

                // TODO: Handle Variadic __VA_ARGS__ mapping here if index is high

                // Paste the argument tokens instead of the parameter name
                PPTokenVector arg_tokens = args.data[param_idx];

                // C Standard: Argument Prescan
                // We should technically expand the argument HERE before pasting,
                // UNLESS the next/prev token is '#' or '##'.
                // For simplicity now, let's paste RAW and let the recursive expand catch it.
                // (This is "Lazy Expansion" - slightly non-standard but simpler for step 1)
                for (size_t k = 0; k < arg_tokens.count; k++) {
                    vector_push(&pasted_tokens, arg_tokens.data[k]);
                }
                continue;
            }
        }

        // Not a parameter? Just copy the body token.
        vector_push(&pasted_tokens, pptoken);
    }

    // 5. Recursive Expansion (Rescan)
    ExpandedTokenVector final_expanded = expand(pasted_tokens);

    // 6. Fix Traceability
    // The tokens in 'final_expanded' think they came from 'pasted_tokens'.
    // We tell them: "You are part of this Macro Invocation"
    for (size_t k = 0; k < final_expanded.count; k++) {
        if (final_expanded.data[k].invocation == nullptr) {
            final_expanded.data[k].invocation = invoc;
        }
    }

    // 7. Unlock
    definition->is_expanding = false;

    return final_expanded;
}

ExpandedTokenVector expand(PPTokenVector pptokens) {
    ExpandedTokenVector expanded_tokens = {0};

    for (size_t i = 0; i < pptokens.count;) {
        if (
            pptoken_is(pptokens.data[i], PP_PUNCTUATOR, "#") &&
            is_at_start_of_line(pptokens, i)
        ) {
            // skip "#" and whitesapce
            i++;
            i = skip_whitespace(pptokens, i);

            // dispatch helpers
            if (pptoken_is(pptokens.data[i], PP_IDENTIFIER, "include")) {
                ExpandedTokenVector included_tokens = handle_include(pptokens, i);
                vector_append(&expanded_tokens, included_tokens);
            }

            else if (pptoken_is(pptokens.data[i], PP_IDENTIFIER, "define")) {
                handle_define(pptokens, i);
            }

            else {
                // it's a null directive and we do nothing
                // just fall through to skip to the next line
            }

            // skip the line
            i = skip_to_next_line(pptokens, i);
        }

        else if (pptokens.data[i].kind == PP_IDENTIFIER) {
            //  need to check if this is a macro invocation
            MacroDefinition* definition = find_macro_definition(pptokens.data[i].spelling);

            // normal token, pass it through
            if (!definition) {
                vector_push(&expanded_tokens, passthrough(pptokens.data + i));
                i++;
                continue;
            }

            if (definition->is_expanding) {
                vector_push(&expanded_tokens, passthrough(pptokens.data + i));
                i++;
                continue;
            }

            if (!definition->is_function_like) {
                ExpandedTokenVector replacement = expand_object_like_macro(definition, pptokens.data + i);
                vector_append(&expanded_tokens, replacement);
                i++;  // eat the name
                continue;
            }

            // now we know it is defined as a function like macro
            size_t snapshot_i = i;
            i++;
            i = skip_whitespace_and_newlines(pptokens, i);
            if (i < pptokens.count && !pptoken_is(pptokens.data[i], PP_PUNCTUATOR, "(")) {
                // restore i;
                i = snapshot_i;

                // push the name
                vector_push(&expanded_tokens, passthrough(pptokens.data + i));
                i++;
                continue;
            }

            // now we know we have to expand it
            i = snapshot_i;
            ExpandedTokenVector replacement = expand_function_like_macro(definition, pptokens, &i);
            vector_append(&expanded_tokens, replacement);
            continue;
        }

        else {
            vector_push(&expanded_tokens, passthrough(pptokens.data + i));
            i++;
        }
    }

    return expanded_tokens;
}
