#include "mcc/frontend/lexer.h"

#include <ctype.h>

#include "mcc/core/error.h"
#include "mcc/frontend/token.h"

typedef struct mcc_frontend_lexer {
    mcc_core_string* string;
    size_t cursor, line, col;
} mcc_frontend_lexer;

mcc_frontend_lexer* mcc_frontend_lexer_construct(mcc_core_arena* arena, mcc_core_string* string) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(arena);
    MCC_CORE_ERROR_FATAL_CHECK_NULL(string);

    mcc_frontend_lexer* self = MCC_CORE_ARENA_ALLOCATE(arena, mcc_frontend_lexer, 1);

    self->cursor = 0;
    self->line = 1;
    self->col = 1;
    self->string = string;

    return self;
}

static void _mcc_frontend_lexer_clear(mcc_frontend_lexer* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    self->cursor = 0;
    self->line = 1;
    self->col = 1;
}

static bool _mcc_frontend_lexer_in_bound(mcc_frontend_lexer* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    return self->cursor < mcc_core_string_get_len(self->string);
}

static char _mcc_frontend_lexer_current(mcc_frontend_lexer* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    // this function should not be called in an out of bound state
    if (!_mcc_frontend_lexer_in_bound(self)) {
        MCC_CORE_ERROR_UNREACHABLE();
    }

    return mcc_core_string_at(self->string, self->cursor);
}

static void _mcc_frontend_lexer_advance(mcc_frontend_lexer* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    if (_mcc_frontend_lexer_current(self) == '\n') {
        self->line++;
        self->col = 1;
    } else {
        self->col++;
    }

    self->cursor++;
}

static void _mcc_frontend_lexer_lex_whitespace(mcc_frontend_lexer* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    for (; _mcc_frontend_lexer_in_bound(self) &&
           isspace(_mcc_frontend_lexer_current(self));
         _mcc_frontend_lexer_advance(self));
}

static bool _mcc_frontend_lexer_isalnum(char c) {
    return isalnum(c) || (c == '_');
}

static bool _mcc_frontend_lexer_isalpha(char c) {
    return isalpha(c) || (c == '_');
}

static mcc_frontend_token* _mcc_frontend_lexer_lex_word(mcc_core_arena* arena, mcc_frontend_lexer* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(arena);
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    // snapshot
    size_t begin = self->cursor;
    size_t line = self->line;
    size_t col = self->col;

    size_t len = 0;
    for (; _mcc_frontend_lexer_in_bound(self) &&
           _mcc_frontend_lexer_isalnum(_mcc_frontend_lexer_current(self));
         _mcc_frontend_lexer_advance(self)) {
        len++;
    }

    mcc_core_string* ident = mcc_core_string_substring(arena, self->string, begin, len);
    mcc_frontend_token* token = mcc_frontend_token_construct(arena, ident, line, col);

    return token;
}

mcc_core_vector* mcc_frontend_lexer_tokenize(mcc_core_arena* arena, mcc_frontend_lexer* self) {
    MCC_CORE_ERROR_FATAL_CHECK_NULL(arena);
    MCC_CORE_ERROR_FATAL_CHECK_NULL(self);

    mcc_core_vector* tokens = mcc_core_vector_construct(arena);

    for (_mcc_frontend_lexer_clear(self); _mcc_frontend_lexer_in_bound(self);) {
        char c = _mcc_frontend_lexer_current(self);

        if (isspace(c)) {
            _mcc_frontend_lexer_lex_whitespace(self);
        }

        else if (_mcc_frontend_lexer_isalpha(c)) {
            mcc_frontend_token* token = _mcc_frontend_lexer_lex_word(arena, self);
            mcc_core_vector_push(tokens, token);
        }

        else {
            _mcc_frontend_lexer_advance(self);
        }
    }

    return tokens;
}
