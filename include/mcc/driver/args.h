#ifndef MCC_DRIVER_ARGS_H
#define MCC_DRIVER_ARGS_H

#include "mcc/core/arena.h"
#include "mcc/core/string.h"

/**
 * @file args.h
 * @brief Command Line Argument Parser.
 *
 * This module is responsible for parsing standard C command line arguments (argc/argv)
 * into a structured, validated configuration object used by the compiler driver.
 */

/* * Opaque handle to the arguments object.
 * The internal layout is hidden to ensure encapsulation.
 * Instances must be allocated via the constructor.
 */
typedef struct mcc_driver_args mcc_driver_args;

// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------

/**
 * @brief Parses command line arguments and constructs the configuration object.
 *
 * Iterates through the provided `argv`, identifies flags (like -o, --help),
 * and validates the inputs.
 *
 * @param arena  The memory arena used to allocate the struct and its internal strings.
 * The lifetime of the returned object is tied to this arena.
 * @param argc   The argument count (passed from main).
 * @param argv   The argument vector (passed from main).
 * * @return A valid pointer to the constructed `mcc_driver_args` object.
 * * @note This function is **Fatal**. If validation fails (e.g., missing input file,
 * unknown flag), it will print an error message to stderr and exit the program.
 * It never returns NULL.
 */
mcc_driver_args* mcc_driver_args_construct(mcc_core_arena* arena, int argc, char** argv);

// ----------------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------------

/**
 * @brief Retrieves the main input source file path.
 *
 * @param self A valid pointer to the driver args object.
 * * @return A pointer to the string view representing the filename (e.g., "test.c").
 * The data is owned by the Arena; do not free it.
 */
mcc_core_string* mcc_driver_args_get_input_file_name(mcc_driver_args* self);

#endif  //  MCC_DRIVER_ARGS_H