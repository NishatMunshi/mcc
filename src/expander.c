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

static void record_params(MacroDefinition* def, PPTokenStream* stream) {
    // Eat the opening parenthesis.
    stream_consume(stream, 1);
    PPTokenVector* params = ARENA_ALLOC(PPTokenVector, 1);

    // Check empty param list
    stream_skip_whitespace(stream);
    if (pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, ")")) {
        stream_consume(stream, 1);
        def->params = params;
        return;
    }

    while (true) {
        stream_skip_whitespace(stream);

        if (stream_peekahead(stream, 0)->kind == PP_NEWLINE ||
            stream_peekahead(stream, 0)->kind == PP_EOF) {
            panic("expected `)`");
        }

        if (pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, "...")) {
            def->is_variadic = true;
            stream_consume(stream, 1);
            stream_skip_whitespace(stream);

            if (!pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, ")")) {
                panic("expected `)`");
            }

            else {
                stream_consume(stream, 1);
                break;
            }
        }

        else if (stream_peekahead(stream, 0)->kind == PP_IDENTIFIER) {
            vector_push(params, stream_peekahead(stream, 0));
            stream_consume(stream, 1);
            stream_skip_whitespace(stream);

            if (pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, ",")) {
                stream_consume(stream, 1);
                continue;
            }

            else if (pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, ")")) {
                stream_consume(stream, 1);
                break;
            }

            else {
                panic("expected `,` or `)`");
            }
        }

        else {
            panic("expected identifier or `...`");
        }
    }

    def->params = params;
}

static void record_function_like_macro(PPTokenStream* stream) {
    MacroDefinition* def = ARENA_ALLOC(MacroDefinition, 1);

    def->name = stream_peekahead(stream, 0)->spelling;
    stream_consume(stream, 1);

    def->is_function_like = true;
    record_params(def, stream);

    stream_skip_whitespace(stream);
    def->replacement_list = gather_macro_replacement_tokens(stream);

    vector_push(&g_expander_context.macro_definitions, def);
    stream_skip_line(stream);
}

static void record_object_like_macro(PPTokenStream* stream) {
    MacroDefinition* def = ARENA_ALLOC(MacroDefinition, 1);

    def->name = stream_peekahead(stream, 0)->spelling;

    stream_consume(stream, 1);
    stream_skip_whitespace(stream);

    def->replacement_list = gather_macro_replacement_tokens(stream);

    vector_push(&g_expander_context.macro_definitions, def);
    stream_skip_line(stream);
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

    if (pptoken_is(stream_peekahead(stream, 1), PP_PUNCTUATOR, "(")) {
        record_function_like_macro(stream);
    }

    else {
        record_object_like_macro(stream);
    }
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

static void stream_skip_whitespace_and_newline(PPTokenStream* stream) {
    while (true) {
        PPTokenKind kind = stream_peekahead(stream, 0)->kind;

        if (kind == PP_WHITESPACE || kind == PP_NEWLINE) {
            stream_consume(stream, 1);
            continue;
        }

        else {
            return;
        }
    }
}

static PPTokenVector* record_arg(PPTokenStream* stream) {
    PPTokenVector* arg = ARENA_ALLOC(PPTokenVector, 1);

    size_t paren_depth = 0;
    while (true) {
        PPToken* token = stream_peekahead(stream, 0);

        if (token->kind == PP_EOF) {
            panic("expected `)`");
        }

        if (pptoken_is(token, PP_PUNCTUATOR, ")") && paren_depth == 0) {
            break;
        }

        if (pptoken_is(token, PP_PUNCTUATOR, ",") && paren_depth == 0) {
            break;
        }

        if (pptoken_is(token, PP_PUNCTUATOR, ")") && paren_depth > 0) {
            paren_depth--;
            vector_push(arg, token);
            stream_consume(stream, 1);
            continue;
        }

        if (pptoken_is(token, PP_PUNCTUATOR, "(")) {
            paren_depth++;
            vector_push(arg, token);
            stream_consume(stream, 1);
            continue;
        }

        vector_push(arg, token);
        stream_consume(stream, 1);
        continue;
    }

    return arg;
}

static PPTokenVector* record_va_args(PPTokenStream* stream) {
    PPTokenVector* va_args = ARENA_ALLOC(PPTokenVector, 1);

    size_t paren_depth = 0;
    while (true) {
        PPToken* token = stream_peekahead(stream, 0);

        if (token->kind == PP_EOF) {
            panic("expected `)`");
        }

        else if (pptoken_is(token, PP_PUNCTUATOR, ")") && paren_depth == 0) {
            break;
        }

        else if (pptoken_is(token, PP_PUNCTUATOR, ")") && paren_depth > 0) {
            paren_depth--;
            vector_push(va_args, token);
            stream_consume(stream, 1);
            continue;
        }

        else if (pptoken_is(token, PP_PUNCTUATOR, "(")) {
            paren_depth++;
            vector_push(va_args, token);
            stream_consume(stream, 1);
            continue;
        }

        else {
            vector_push(va_args, token);
            stream_consume(stream, 1);
            continue;
        }
    }

    return va_args;
}

static PPTokenVectorVector* record_args(MacroDefinition* def, PPTokenStream* stream) {
    // Eat the opening parenthesis
    stream_consume(stream, 1);

    PPTokenVectorVector* args = ARENA_ALLOC(PPTokenVectorVector, 1);

    // This loop grabs the first N - 1 REQUIRED args.
    for (size_t i = 0; i + 1 < def->params->count; ++i) {
        stream_skip_whitespace_and_newline(stream);

        PPTokenVector* reqd_arg = record_arg(stream);
        vector_push(args, reqd_arg);

        if (!pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, ",")) {
            panic("expected `,`");
        }

        stream_consume(stream, 1);
        continue;
    }

    stream_skip_whitespace_and_newline(stream);

    // Grab the last REQUIRED arg
    if (args->count < def->params->count) {
        PPTokenVector* last_reqd_arg = record_arg(stream);
        vector_push(args, last_reqd_arg);
    }

    // branch depending on whether we expect va_args
    if (!def->is_variadic) {
        if (!pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, ")")) {
            panic("expected `)`");
        }

        stream_consume(stream, 1);
        return args;
    }

    else {
        if (pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, ",")) {
            stream_consume(stream, 1);

            PPTokenVector* va_args = record_va_args(stream);
            vector_push(args, va_args);
        }

        // Expect a `)`
        if (!pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, ")")) {
            printf("here\n");
            panic("expected `)`");
        }

        stream_consume(stream, 1);
        return args;
    }
}

static ssize_t params_contains(PPTokenVector* params, PPToken* param) {
    for (ssize_t i = 0; i < (ssize_t)params->count; ++i) {
        if (pptoken_is(params->data[i], PP_IDENTIFIER, param->spelling)) {
            return i;
        }
    }

    return -1;
}

static PPTokenVector* record_va_opt_tokens(PPTokenStream* stream) {
    stream_consume(stream, 1);  // eat __VA_OPT__
    stream_skip_whitespace(stream);

    if (!pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, "(")) {
        panic("expected `(`");
    }

    stream_consume(stream, 1);

    PPTokenVector* va_opt_tokens = ARENA_ALLOC(PPTokenVector, 1);

    size_t paren_depth = 0;
    while (true) {
        PPToken* token = stream_peekahead(stream, 0);

        if (token->kind == PP_EOF) {
            panic("expected `)`");
        }

        else if (pptoken_is(token, PP_PUNCTUATOR, ")") && paren_depth == 0) {
            stream_consume(stream, 1);
            break;
        }

        else if (pptoken_is(token, PP_PUNCTUATOR, ")") && paren_depth > 0) {
            paren_depth--;
            vector_push(va_opt_tokens, token);
            stream_consume(stream, 1);
            continue;
        }

        else if (pptoken_is(token, PP_PUNCTUATOR, "(")) {
            paren_depth++;
            vector_push(va_opt_tokens, token);
            stream_consume(stream, 1);
            continue;
        }

        else {
            vector_push(va_opt_tokens, token);
            stream_consume(stream, 1);
            continue;
        }
    }

    return va_opt_tokens;
}

static PPTokenVector* replace_params(MacroDefinition* def, PPTokenVectorVector* args) {
    PPTokenVector* template = def->replacement_list;
    PPTokenVector* params = def->params;

    PPTokenVector* new_pptokens = ARENA_ALLOC(PPTokenVector, 1);

    PPTokenStream stream = {
        .pptokens = template,
        .current_index = 0,
    };

    while (true) {
        PPToken* template_token = stream_peekahead(&stream, 0);

        if (template_token->kind == PP_EOF) {
            break;
        }

        if (template_token->kind != PP_IDENTIFIER) {
            vector_push(new_pptokens, template_token);
            stream_consume(&stream, 1);
            continue;
        }

        // now we know it IS an identifier
        if (pptoken_is(template_token, PP_IDENTIFIER, "__VA_ARGS__")) {
            if (!def->is_variadic) {
                panic("__VA_ARGS__ inside nonvariadic macro");
            }

            vector_append(new_pptokens, args->data[args->count - 1]);
            stream_consume(&stream, 1);
        }

        else if (pptoken_is(template_token, PP_IDENTIFIER, "__VA_OPT__")) {
            if (!def->is_variadic) {
                panic("__VA_OPT__ inside nonvariadic macro");
            }

            PPTokenVector* va_opt_tokens = record_va_opt_tokens(&stream);
            PPTokenVector* va_args = args->data[args->count - 1];

            if (va_args->count > 0) {
                vector_append(new_pptokens, va_opt_tokens);
            }

            else {
                continue;
            }
        }

        else {
            ssize_t param_index = params_contains(params, template_token);

            if (param_index == -1) {
                // not a param, push it
                vector_push(new_pptokens, template_token);
                stream_consume(&stream, 1);
                continue;
            }

            PPTokenVector* arg = args->data[param_index];
            vector_append(new_pptokens, arg);
            stream_consume(&stream, 1);
        }
    }

    return new_pptokens;
}

static ExpandedTokenVector* expand_function_like_macro(MacroDefinition* def, PPTokenStream* stream) {
    PPToken* macro_name_token = stream_peekahead(stream, 0);
    stream_consume(stream, 1);

    MacroInvocation* invoc = ARENA_ALLOC(MacroInvocation, 1);
    invoc->definition = def;
    invoc->origin = macro_name_token;

    stream_skip_whitespace_and_newline(stream);

    if (!pptoken_is(stream_peekahead(stream, 0), PP_PUNCTUATOR, "(")) {
        panic("expected `(`");
    }

    invoc->arguments = record_args(def, stream);

    PPTokenVector* replacement_tokens = replace_params(def, invoc->arguments);

    // rescan
    ExpandedTokenVector* expanded_tokens = expand(replacement_tokens);

    // Stamp the invocation instance
    for (size_t i = 0; i < expanded_tokens->count; ++i) {
        ExpandedToken* token = expanded_tokens->data[i];
        if (token->invocation == nullptr) {
            token->invocation = invoc;
        }
    }

    return expanded_tokens;
}

static ExpandedTokenVector* expand_object_like_macro(MacroDefinition* def, PPTokenStream* stream) {
    PPToken* macro_name_token = stream_peekahead(stream, 0);
    stream_consume(stream, 1);

    MacroInvocation* invoc = ARENA_ALLOC(MacroInvocation, 1);
    invoc->definition = def;
    invoc->origin = macro_name_token;

    // rescan
    ExpandedTokenVector* expanded_tokens = expand(def->replacement_list);

    // Stamp the invocation instance
    for (size_t i = 0; i < expanded_tokens->count; ++i) {
        ExpandedToken* token = expanded_tokens->data[i];
        if (token->invocation == nullptr) {
            token->invocation = invoc;
        }
    }

    return expanded_tokens;
}

static ExpandedTokenVector* expand_macro(PPTokenStream* stream) {
    PPToken* macro_name_token = stream_peekahead(stream, 0);
    char* macro_name = macro_name_token->spelling;
    MacroDefinition* def = is_defined(macro_name);

    if (def->is_function_like) {
        return expand_function_like_macro(def, stream);
    }

    else {
        return expand_object_like_macro(def, stream);
    }
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
        // because it may be a conditional directive,
        if (pptoken_is(pptoken, PP_PUNCTUATOR, "#") && stream_is_it_start_of_line(&stream)) {
            ExpandedTokenVector* new_expanded_tokens = execute_directive(&stream);
            vector_append(expanded_tokens, new_expanded_tokens);
            continue;
        }

        ConditionalState current_conditonal_state = conditional_stack_top(&g_expander_context.conditional_stack);

        // If we are skipping, just skip and move on.
        if (current_conditonal_state == COND_SKIPPING) {
            stream_skip_line(&stream);
            continue;
        }

        // Check if it is a macro invocation.
        if (pptoken->kind == PP_IDENTIFIER && is_defined(pptoken->spelling)) {
            ExpandedTokenVector* new_expanded_tokens = expand_macro(&stream);
            vector_append(expanded_tokens, new_expanded_tokens);
            continue;
        }

        // This is a normal pptoken. Take it as it is.
        ExpandedToken* expanded_token = ARENA_ALLOC(ExpandedToken, 1);
        expanded_token->kind = pptoken->kind;
        expanded_token->spelling = strdup(pptoken->spelling);
        expanded_token->length = pptoken->length;
        expanded_token->origin = pptoken;
        expanded_token->invocation = nullptr;

        vector_push(expanded_tokens, expanded_token);
        stream_consume(&stream, 1);
    }

    return expanded_tokens;
}
