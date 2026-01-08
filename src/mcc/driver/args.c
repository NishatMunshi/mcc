#include "mcc/driver/args.h"

#include <stdio.h>
#include <stdlib.h>

#include "mcc/core/error.h"
#include "mcc/core/string.h"

#define _MCC_DRIVER_ARGS_VERSION ("mcc version 0.0.1\n")

struct mcc_driver_args {
    mcc_core_string *input_file_name;
    mcc_core_string *output_file_name;
};

static void _mcc_driver_args_print_usage(int exit_code) {
     char *usage_cstr =
        "Usage: mcc [options] file...\n"
        "Options:\n"
        "  --help             Display this information.\n"
        "  --version          Display compiler version information.\n"
        "  -o <file>          Place the output into <file>.\n";

    FILE *stream = exit_code ? stderr : stdout;
    fprintf(stream, "%s", usage_cstr);
    exit(exit_code);
}

mcc_driver_args *mcc_driver_args_construct(mcc_core_arena *arena, int argc, char **argv) {
    MCC_CORE_ERROR_CHECK_NULL(arena);
    MCC_CORE_ERROR_CHECK_NULL(argv);
    if (argc < 1) mcc_core_error_fatal("mcc_driver_args_construct: argc < 1");

    mcc_driver_args *self = MCC_CORE_ARENA_ALLOCATE(arena, mcc_driver_args, 1);

    bool parsing_options = true;
    for (int i = 1; i < argc; ++i) {
        mcc_core_string *arg = mcc_core_string_construct_copy_from_cstr(arena, argv[i]);

        // --- Option Parsing ---
        if (parsing_options && mcc_core_string_starts_with_cstr(arg, "-")) {
            if (mcc_core_string_equals_cstr(arg, "--")) {
                // this is the "end of options" flag
                parsing_options = false;
                continue;
            }

            if (mcc_core_string_equals_cstr(arg, "--help")) {
                _mcc_driver_args_print_usage(0);
            }

            else if (mcc_core_string_equals_cstr(arg, "--version")) {
                printf(_MCC_DRIVER_ARGS_VERSION);
                exit(0);
            }

            else if (mcc_core_string_equals_cstr(arg, "-o")) {
                // Lookahead check
                if (i + 1 >= argc) {
                    mcc_core_error_fatal("missing filename after '-o'");
                }

                // Consume the next argument immediately
                i++;
                self->output_file_name = mcc_core_string_construct_copy_from_cstr(arena, argv[i]);
            }

            else {
                mcc_core_error_fatal("unknown option provided: %s", argv[i]);
            }
        }

        // --- Input File Parsing ---
        else {
            if (self->input_file_name != NULL) {
                mcc_core_error_fatal("multiple input files provided");
            }
            self->input_file_name = arg;
        }
    }

    // Final Validation
    if (self->input_file_name == NULL) {
        mcc_core_error_fatal("no input files");
    }

    if (self->output_file_name == NULL) {
        self->output_file_name = mcc_core_string_construct_copy_from_cstr(arena, "a.s");
    }

    return self;
}

// getters
mcc_core_string *mcc_driver_args_get_input_file_name(mcc_driver_args *self) {
    MCC_CORE_ERROR_CHECK_NULL(self);

    return self->input_file_name;
}