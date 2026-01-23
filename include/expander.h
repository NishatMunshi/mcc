#ifndef EXPANDER_H
#define EXPANDER_H

#include "tokenizer.h"

typedef struct MacroDefinition {
    char* name;

    bool is_function_like;
    PPTokenVector* params;
    bool is_variadic;

    PPTokenVector* replacement_list;
} MacroDefinition;

typedef struct PPTokenVectorVector {
    PPTokenVector** data;
    size_t count;
    size_t capacity;
} PPTokenVectorVector;

typedef struct MacroInvocation {
    MacroDefinition* definition;

    PPToken* origin;

    PPTokenVectorVector* arguments;
} MacroInvocation;

typedef struct ExpandedToken {
    PPTokenKind kind;
    char* spelling;
    size_t length;

    // 1. The Physical Source (Spelling Location)
    // - For normal code: Points to the token in the source file.
    // - For macro body: Points to the token INSIDE the #define definition.
    // - For macro args: Points to the token in the argument list.
    PPToken* origin;

    // 2. The Logical Source (Expansion History)
    // If this token appeared because of a macro, this points to that call.
    // NULL if this token appeared directly in the source code.
    MacroInvocation* invocation;
} ExpandedToken;

typedef struct ExpandedTokenVector {
    ExpandedToken** data;

    size_t count;
    size_t capacity;
} ExpandedTokenVector;

ExpandedTokenVector* expand(PPTokenVector* pp_tokens);

#endif  // EXPANDER_H
