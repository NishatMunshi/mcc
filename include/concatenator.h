#ifndef CONCATENATOR_H
#define CONCATENATOR_H

#include "converter.h"

typedef struct CToken CToken;
typedef struct CTokenVector CTokenVector;

CTokenVector* concatenate(ConvertedTokenVector* converted_tokens);

#endif  // CONCATENATOR_H
