#ifndef EXPANDER_H
#define EXPANDER_H

#include "tokenizer.h"

typedef struct MacroDefinition MacroDefinition;
typedef struct MacroInvocation MacroInvocation;
typedef struct ExpandedToken ExpandedToken;
typedef struct ExpandedTokenVector ExpandedTokenVector;

ExpandedTokenVector* expand(PPTokenVector* pp_tokens);

#endif  // EXPANDER_H
