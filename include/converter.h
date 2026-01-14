#ifndef CONVERTER_H
#define CONVERTER_H

#include "expander.h"

typedef struct ConvertedToken ConvertedToken;
typedef struct ConvertedTokenVector ConvertedTokenVector;

ConvertedTokenVector* convert(ExpandedTokenVector* expanded_tokens);

#endif  // CONVERTER_H
