#ifndef EXPANDER_H
#define EXPANDER_H

#include "tokenizer.h"

typedef struct MacroDefinition MacroDefinition;
typedef struct MacroInvocation MacroInvocation;

typedef struct ExpandedToken {
    PPTokenKind kind;
    char* spelling;
    size_t length;

    PPToken* pptoken;
} ExpandedToken;

typedef struct ExpandedTokenVector {
    ExpandedToken* data;

    size_t count;
    size_t capacity;
} ExpandedTokenVector;

ExpandedTokenVector expand(PPTokenVector pp_tokens);

#endif  // EXPANDER_H
