#include "tokenizer.h"

#include "arena.h"
#include "diag.h"
#include "file.h"
#include "vector.h"

typedef struct Tokenizer {
    File* file;
    size_t index;
    size_t logic_line;
    size_t logic_col;
} Tokenizer;

Token* token_create(Tokenizer* tokenizer, TokenType type) {
    Token* tok = ARENA_ALLOC(Token, 1);
    tok->file = tokenizer->file;
    tok->logic_col = tokenizer->logic_col;
    tok->logic_line = tokenizer->logic_line;
    tok->text_start = tokenizer->file->spl_data + tokenizer->index;
    tok->text_len = 1;
    tok->type = type;

    return tok;
}

Token* tokenize_single_line_comment(Tokenizer* tokenizer) {
    Token* tok = token_create(tokenizer, TOK_WHITESPACE);
    
    // guaranteed to hit newline after phase 2
    size_t text_len = 0;
    while (tokenizer->file->spl_data[tokenizer->index] != '\n') {
        tokenizer->logic_col++;
        tokenizer->index++;
        text_len++;
    }
    
    tok->text_len = text_len;
    return tok;
}

Token* tokenize_block_comment(Tokenizer* tokenizer) {
    Token* tok = token_create(tokenizer, TOK_WHITESPACE);
    
    size_t text_len = 0;
    while (
        !(tokenizer->file->spl_data[tokenizer->index] == '*' &&
            tokenizer->file->spl_data[tokenizer->index + 1] == '/')
        ) {
            if (tokenizer->file->spl_data[tokenizer->index] == '\0') {
                /*
                A source file shall not end in a partial preprocessing token or in a partial comment.
                */
            tok->text_len = text_len;
            diag_error(tok, "unterminated block comment");
        }
        
        // newline inside block comment
        if (tokenizer->file->spl_data[tokenizer->index] == '\n') {
            tokenizer->logic_col = 1;
            tokenizer->logic_line++;
        } else {
            tokenizer->logic_col++;
        }
        
        tokenizer->index++;
        text_len++;
    }
    
    tok->text_len = text_len + 2;
    return tok;
}

bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\v' || c == '\f';
}

Token* tokenize_whitespace(Tokenizer* tokenizer) {
    Token* tok = token_create(tokenizer, TOK_WHITESPACE);

    size_t text_len = 0;
    while (is_space(tokenizer->file->spl_data[tokenizer->index])) {
        tokenizer->logic_col++;
        tokenizer->index++;
        text_len++;
    }

    tok->text_len = text_len;
    return tok;
}

TokenVector* tokenize(File* file) {
    TokenVector* vec = ARENA_ALLOC(TokenVector, 1);

    char* string = file->spl_data;

    Tokenizer tokenizer = {
        .file = file,
        
        .index = 0,
        .logic_line = 1,
        .logic_col = 1,
    };

    /*
    Splicer guarantees null terminated output
    Lookahead by 1 is safe
    */
    while (tokenizer.index < file->spl_size) {
        /*
        Each comment is replaced by one space character.
        */
        // single line comments
        if (string[tokenizer.index] == '/' && string[tokenizer.index + 1] == '/') {
            Token* tok = tokenize_single_line_comment(&tokenizer);
            vector_push(vec, tok);
            continue;
        }

        /* BLOCK COMMENTS*/
        else if (string[tokenizer.index] == '/' && string[tokenizer.index + 1] == '*') {
            Token* tok = tokenize_block_comment(&tokenizer);
            vector_push(vec, tok);
            continue;
        }

        /*
        New-line characters are retained.
        */
        else if (string[tokenizer.index] == '\n') {
            Token* tok = token_create(&tokenizer, TOK_NEWLINE);
            vector_push(vec, tok);

            tokenizer.logic_col = 1;
            tokenizer.logic_line++;
            tokenizer.index++;
            continue;
        }

        /*
        Whether each nonempty sequence of white-space characters
        other than new-line is retained or
        replaced by one space character is implementation-defined.

        We replace it as with one space character
        */
        else if (is_space(string[tokenizer.index])) {
            Token* tok = tokenize_whitespace(&tokenizer);
            vector_push(vec, tok);
            continue;
        }

        else {
            Token* tok = token_create(&tokenizer, TOK_ERROR);

            size_t text_len = 0;
            while (
                !(string[tokenizer.index] == '/' && string[tokenizer.index + 1] == '/') &&
                !(string[tokenizer.index] == '/' && string[tokenizer.index + 1] == '*') &&
                !(string[tokenizer.index] == '\n') &&
                !(is_space(string[tokenizer.index]))
            ) {
                tokenizer.logic_col++;
                tokenizer.index++;
                text_len++;
            }

            tok->text_len = text_len;
            vector_push(vec, tok);
            continue;
        }
    }

    return vec;
}
