#ifndef MCC_FRONTEND_TOKEN_H
#define MCC_FRONTEND_TOKEN_H

#include "mcc/core/arena.h"
#include "mcc/core/string.h"

typedef struct mcc_frontend_token mcc_frontend_token;

mcc_frontend_token* mcc_frontend_token_construct(mcc_core_arena* arena, mcc_core_string* ident, size_t line, size_t col);
void mcc_frontend_token_print(mcc_frontend_token* token, FILE* stream);

#endif  // MCC_FRONTEND_TOKEN_H
