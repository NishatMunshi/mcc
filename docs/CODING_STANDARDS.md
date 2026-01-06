# MCC Coding Standards

## 1. Object Oriented C
* **Opaque Types:** All structs representing "classes" must be opaque in the `.h` file.
    * *Header:* `typedef struct mcc_module_class mcc_module_class;`
    * *Source:* `struct mcc_module_class { ... };` defines the internals.
* **Encapsulation:** No direct member access from outside the module. Use getters/setters.
* **One Class Per File:** Every `.c` file represents exactly one class (struct) or a tightly coupled set of utility functions.

## 2. Dependencies
* **Forbidden:** No external libraries (boost, glib, etc.).
* **Allowed:** C Standard Library (`<stdlib.h>`, `<stdio.h>`, `<stdint.h>`, etc.) ONLY.
* **Internal:** Use `mcc/core/` for Strings, Vectors, and Memory Management.

## 3. Directory & Includes
* **Split:** Implementations in `src/`, definitions in `include/`.
* **Paths:** Includes must utilize the full path relative to the `include/` directory.
    * Correct: `#include "mcc/core/vector.h"`
    * Incorrect: `#include "vector.h"` or `#include "../core/vector.h"`

## 4. Header Guards
* All headers must use strictly formatted `#ifndef` guards matching the path.
* Format: `MCC_MODULE_SUBMODULE_FILENAME_H`

## 5. Memory Management
* **Arenas:** The compiler is a batch process. Prefer Arena allocation (in `mcc_core_arena`) over individual `malloc/free` to simplify cleanup and improve performance.

## 6. Formatting
* Brace style: K&R or 1TBS.
* Indentation: 4 Spaces (No tabs).
* No inline comments on the same line as code.