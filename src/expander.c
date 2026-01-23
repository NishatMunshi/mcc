#include <arena.h>
#include <expander.h>
#include <io.h>
#include <panic.h>
#include <string.h>
#include <tokenizer.h>
#include <types.h>
#include <vector.h>

typedef enum ConditionalState {
    COND_SKIPPING,
    COND_ACTIVE,
    COND_SATISFIED,
} ConditionalState;

typedef struct ConditionalStack {
    ConditionalState* data;
    size_t count;
    size_t capacity;
} ConditionalStack;

typedef struct MacroDefinitionMap {
    MacroDefinition** data;


    size_t count;
    size_t capacity;
} MacroDefinitionMap;

typedef struct ExpanderContext {
    char* LIB_DIR;
    size_t MAX_INCLUDE_DEPTH;

    size_t current_include_depth;
    MacroDefinitionMap macro_definitions;
    ConditionalStack conditional_stack;
} ExpanderContext;

typedef struct PPTokenStream {
    PPTokenVector* pptokens;
    size_t current_index;
} PPTokenStream;

static ExpanderContext g_expander_context = {
    .LIB_DIR = "./include/",
    .MAX_INCLUDE_DEPTH = 15,

    .current_include_depth = 0,
    .macro_definitions = {0},
    .conditional_stack = {0},
};

static void conditional_stack_push(ConditionalStack* stack, ConditionalState state) {
    vector_push(stack, state);
}

static void conditional_stack_pop(ConditionalStack* stack) {
    if (stack->count == 0) {
        return;
    } else {
        stack->count -= 1;
    }
}

static ConditionalState conditional_stack_top(ConditionalStack* stack) {
    if (stack->count == 0) {
        return COND_ACTIVE;
    }

    else {
        return stack->data[stack->count - 1];
    }
}

static MacroDefinition* is_defined(char* macro_name) {
    for (size_t i = 0; i < g_expander_context.macro_definitions.count; ++i) {
        if (streq(g_expander_context.macro_definitions.data[i]->name, macro_name)) {
            return g_expander_context.macro_definitions.data[i];
        }
    }

    return nullptr;
}

static PPToken* stream_peekahead(PPTokenStream* stream, ssize_t offset) {
    static PPToken EOF_SENTINEL = {
        .kind = PP_EOF,
        .length = 0,
        .origin = nullptr,
        .spelling = nullptr,
    };

    ssize_t target_index = (ssize_t)stream->current_index + offset;

    if (target_index >= (ssize_t)stream->pptokens->count || target_index < 0) {
        return &EOF_SENTINEL;
    }

    return stream->pptokens->data[target_index];
}

static void stream_consume(PPTokenStream* stream, size_t count) {
    if (stream->current_index + count >= stream->pptokens->count) {
        stream->current_index = stream->pptokens->count;
    }

    else {
        stream->current_index += count;
    }
}

static bool stream_is_it_start_of_line(PPTokenStream* stream) {
    ssize_t i = -1;
    while (stream_peekahead(stream, i)->kind == PP_WHITESPACE) {
        i--;
    }

    PPTokenKind kind = stream_peekahead(stream, i)->kind;

    if (kind == PP_EOF || kind == PP_NEWLINE) {
        return true;
    }

    else {
        return false;
    }
}

static void stream_skip_line(PPTokenStream* stream) {
    while (true) {
        PPTokenKind kind = stream_peekahead(stream, 0)->kind;

        if (kind == PP_NEWLINE || kind == PP_EOF) {
            stream_consume(stream, 1);
            break;
        }

        else {
            stream_consume(stream, 1);
            continue;
        }
    }
}

static void stream_skip_whitespace(PPTokenStream* stream) {
    while (true) {
        PPTokenKind kind = stream_peekahead(stream, 0)->kind;

        if (kind == PP_WHITESPACE) {
            stream_consume(stream, 1);
            continue;
        }

        else {
            break;
        }
    }
}

static char* strip_delims(PPToken* header_name) {
    size_t new_len = header_name->length - 2;
    char* buffer = ARENA_ALLOC(char, new_len + 1);

    for (size_t i = 0; i < new_len; ++i) {
        buffer[i] = header_name->spelling[i + 1];
    }

    buffer[new_len] = '\0';

    return buffer;
}

static ssize_t find_last_of(char* string, char c) {
    ssize_t index = strlen(string) - 1;

    while (index >= 0 && string[index] != c) {
        index--;
    }

    return index;
}

static char* full_path_to_dir(char* full_path) {
    ssize_t last_slash_index = find_last_of(full_path, '/');

    if (last_slash_index == -1) {
        return strdup("./");
    }

    size_t length = last_slash_index + 1;
    char* buf = ARENA_ALLOC(char, length + 1);

    for (size_t i = 0; i < length; ++i) {
        buf[i] = full_path[i];
    }

    buf[length] = '\0';
    return buf;
}

static char* get_header_full_path(PPToken* header_name) {
    char* header_relative_path = strip_delims(header_name);
    char* header_full_path = nullptr;

    if (header_name->spelling[0] == '<') {
        // it is a library header.
        header_full_path = strcat(g_expander_context.LIB_DIR, header_relative_path);
    }

    else {
        char* this_file_full_path = header_name->origin->data[0]->source_char->origin->data[0]->origin->definition->full_path;
        char* this_file_dir = full_path_to_dir(this_file_full_path);
        header_full_path = strcat(this_file_dir, header_relative_path);
    }

    return header_full_path;
}

static ExpandedTokenVector* expand_include(PPTokenStream* stream) {
    if (g_expander_context.current_include_depth >= g_expander_context.MAX_INCLUDE_DEPTH) {
        panic("max include depth reached");
    }

    // We are pointing at the "include" identiifer
    stream_consume(stream, 1);
    stream_skip_whitespace(stream);

    PPToken* header_name = stream_peekahead(stream, 0);

    if (header_name->kind != PP_HEADERNAME) {
        panic("expected header name after `#include`");
    }

    char* header_full_path = get_header_full_path(header_name);

    g_expander_context.current_include_depth++;

    ByteVector* bytes = read(header_full_path, header_name);
    SourceCharVector* source_chars = normalize(bytes);
    SplicedCharVector* spliced_chars = splice(source_chars);
    PPTokenVector* pptokens = tokenize(spliced_chars);
    ExpandedTokenVector* expanded_tokens = expand(pptokens);

    g_expander_context.current_include_depth--;

    // clean up
    stream_skip_line(stream);

    return expanded_tokens;
}

PPTokenVector* gather_macro_replacement_tokens(PPTokenStream* stream) {
    PPTokenVector* replacement_list = ARENA_ALLOC(PPTokenVector, 1);

    while (true) {
        PPToken* pptoken = stream_peekahead(stream, 0);

        if (pptoken->kind == PP_NEWLINE || pptoken->kind == PP_EOF) {
            break;
        }

        else {
            vector_push(replacement_list, pptoken);
            stream_consume(stream, 1);
            continue;
        }
    }

    return replacement_list;
}

static void record_define(PPTokenStream* stream) {
    stream_consume(stream, 1);
    stream_skip_whitespace(stream);

    PPToken* macro_name_token = stream_peekahead(stream, 0);

    if (macro_name_token->kind != PP_IDENTIFIER) {
        panic("expected macro name after `#define`");
    }

    char* macro_name = macro_name_token->spelling;
    if (is_defined(macro_name)) {
        panic("redefinition of macro");
    }

    MacroDefinition* definition = ARENA_ALLOC(MacroDefinition, 1);
    definition->name = strdup(macro_name_token->spelling);
    definition->replacement_list = gather_macro_replacement_tokens(stream);

    vector_push(&g_expander_context.macro_definitions, definition);

    // clean up
    stream_skip_line(stream);
}

static void record_ifndef(PPTokenStream* stream) {
    stream_consume(stream, 1);
    stream_skip_whitespace(stream);

    ConditionalState parent_state = conditional_stack_top(&g_expander_context.conditional_stack);

    ConditionalState my_state = COND_ACTIVE;
    if (parent_state == COND_SKIPPING) {
        my_state = COND_SKIPPING;
    }

    else if (parent_state == COND_ACTIVE) {
        PPToken* macro_name = stream_peekahead(stream, 0);

        if (macro_name->kind != PP_IDENTIFIER) {
            panic("expected macro name after `#ifndef`");
        }

        if (is_defined(macro_name->spelling)) {
            my_state = COND_SKIPPING;
        }

        else {
            my_state = COND_ACTIVE;
        }
    }

    conditional_stack_push(&g_expander_context.conditional_stack, my_state);

    // clean up
    stream_skip_line(stream);
}

static void record_endif(PPTokenStream* stream) {
    if (g_expander_context.conditional_stack.count == 0) {
        panic("stray `#endif`");
    }

    conditional_stack_pop(&g_expander_context.conditional_stack);

    // clean up
    stream_skip_line(stream);
}

static ExpandedTokenVector* execute_directive(PPTokenStream* stream) {
    stream_consume(stream, 1);
    stream_skip_whitespace(stream);

    ConditionalState current_conditional_state = conditional_stack_top(&g_expander_context.conditional_stack);
    PPToken* directive_name_token = stream_peekahead(stream, 0);

    // These directives are only checked if we are not skipping
    if (current_conditional_state != COND_SKIPPING) {
        if (pptoken_is(directive_name_token, PP_IDENTIFIER, "include")) {
            return expand_include(stream);
        }
        
        // else if (#embed, #undef)
        else if (pptoken_is(directive_name_token, PP_IDENTIFIER, "define")) {
            record_define(stream);
        }
    }

    if (pptoken_is(directive_name_token, PP_IDENTIFIER, "ifndef")) {
        record_ifndef(stream);
    }

    else if (pptoken_is(directive_name_token, PP_IDENTIFIER, "endif")) {
        record_endif(stream);
    }

    // else if (#elifndef, #else ...)

    else {
        // null directive or nondirective. Just forget this line.
        stream_skip_line(stream);
    }
    
    return ARENA_ALLOC(ExpandedTokenVector, 1);
}

ExpandedTokenVector* expand(PPTokenVector* pptokens) {
    PPTokenStream stream = {
        .pptokens = pptokens,
        .current_index = 0,
    };

    ExpandedTokenVector* expanded_tokens = ARENA_ALLOC(ExpandedTokenVector, 1);

    while (true) {
        PPToken* pptoken = stream_peekahead(&stream, 0);

        if (pptoken->kind == PP_EOF) {
            break;
        }

        // Directives must be checked even if we are skipping
        // because it may be a conditional directive
        if (pptoken_is(pptoken, PP_PUNCTUATOR, "#") && stream_is_it_start_of_line(&stream)) {
            ExpandedTokenVector* new_expanded_tokens = execute_directive(&stream);
            vector_append(expanded_tokens, new_expanded_tokens);
            continue;
        }

        ConditionalState current_conditonal_state = conditional_stack_top(&g_expander_context.conditional_stack);

        if (current_conditonal_state != COND_SKIPPING) {
            // This is a normal pptoken. take it as it is.
            ExpandedToken* expanded_token = ARENA_ALLOC(ExpandedToken, 1);
            expanded_token->kind = pptoken->kind;
            expanded_token->spelling = strdup(pptoken->spelling);
            expanded_token->length = pptoken->length;
            expanded_token->origin = pptoken;
            expanded_token->invocation = nullptr;

            vector_push(expanded_tokens, expanded_token);
            stream_consume(&stream, 1);
        }

        else {
            stream_skip_line(&stream);
        }
    }

    return expanded_tokens;
}
