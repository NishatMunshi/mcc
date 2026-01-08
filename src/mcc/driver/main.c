#include <stdio.h>

#include "mcc/core/arena.h"
#include "mcc/core/file.h"
#include "mcc/core/string.h"
#include "mcc/core/vector.h"
#include "mcc/driver/args.h"
#include "mcc/frontend/lexer.h"
#include "mcc/frontend/token.h"

int main(int argc, char** argv) {
    // set up compiler environment
    mcc_core_arena* permanent_arena = mcc_core_arena_construct();
    mcc_driver_args* command_line_arguments = mcc_driver_args_construct(permanent_arena, argc, argv);

    // read source file
    mcc_core_string* input_file_name = mcc_driver_args_get_input_file_name(command_line_arguments);
    mcc_core_string* source_string = mcc_core_file_read(permanent_arena, input_file_name);

    // lex and print
    mcc_frontend_lexer* lexer = mcc_frontend_lexer_construct(permanent_arena, source_string);
    mcc_core_vector* tokens = mcc_frontend_lexer_tokenize(permanent_arena, lexer);

    for (size_t i = 0; i < mcc_core_vector_size(tokens); ++i) {
        mcc_frontend_token* token = mcc_core_vector_get(tokens, i);
        mcc_frontend_token_print(token, stdout);
    }

    printf("tokenization complete\n");
    mcc_core_arena_destruct(permanent_arena);
    return 0;
}