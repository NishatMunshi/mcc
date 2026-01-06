# MCC Naming Conventions

## 1. General
* **Case:** Snake case (`lower_case_with_underscores`) for everything.
* **Prefixing:** All public symbols must be prefixed with the full module hierarchy to prevent namespace collisions.

## 2. Functions
* **Public:** `mcc_<module>_<submodule>_<action>`
    * Example: `mcc_core_vector_push(...)`
    * Example: `mcc_backend_x64_emitter_emit(...)`
* **Private:** `_<module>_<submodule>_<action>` (Static to translation unit)
    * Example: `_mcc_frontend_lexer_skip_whitespace(...)`

## 3. Types (Structs/Enums)
* Format: `mcc_<module>_<submodule>_<name>`
    * Example: `mcc_ast_node`
    * Example: `mcc_frontend_token_type`

## 4. Macros
* Format: `MCC_<MODULE>_<SUBMODULE>_<NAME>` (Upper Snake Case)
    * Example: `MCC_CORE_VECTOR_INIT_CAPACITY`

## 5. Variables
* Local variables: Short, descriptive snake_case.
* Global variables: **Strictly Forbidden** unless wrapped in a singleton pattern within the module.