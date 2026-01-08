#include "mcc/frontend/token.h"

#include "mcc/core/error.h"

struct mcc_frontend_token {
    mcc_core_string* ident;
    size_t line, col;
};

mcc_frontend_token* mcc_frontend_token_construct(mcc_core_arena* arena, mcc_core_string* ident, size_t line, size_t col) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(arena);
    MCC_CORE_ERROR_FATAL_CHECK_NULL(ident);
    MCC_CORE_ERROR_FATAL_CHECK_ZERO(line);
    MCC_CORE_ERROR_FATAL_CHECK_ZERO(col);

    mcc_frontend_token* self = MCC_CORE_ARENA_ALLOCATE(arena, mcc_frontend_token, 1);

    // TODO: add look up
    self->ident = ident;

    self->line = line;
    self->col = col;

    return self;
}

void mcc_frontend_token_print(mcc_frontend_token* token, FILE* stream) {
    fprintf(stream, "[%zu:%zu: <", token->line, token->col);
    mcc_core_string_print(token->ident, stream);
    fprintf(stream, ">]\n");
}
