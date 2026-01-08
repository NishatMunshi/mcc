#ifndef MCC_FRONTEND_LEXER_H
#define MCC_FRONTEND_LEXER_H

#include "mcc/core/string.h"
#include "mcc/core/vector.h"

typedef struct mcc_frontend_lexer mcc_frontend_lexer;

mcc_frontend_lexer* mcc_frontend_lexer_construct(mcc_core_arena* arena, mcc_core_string* string);

mcc_core_vector* mcc_frontend_lexer_tokenize(mcc_core_arena* arena, mcc_frontend_lexer* lexer);

#endif  //  MCC_FRONTEND_LEXER_H