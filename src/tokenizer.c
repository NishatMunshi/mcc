#include <tokenizer.h>

#include <arena.h>
#include <io.h>
#include <panic.h>
#include <vector.h>

static PPToken pptoken_create(PPTokenKind kind, SplicedChar* splicedchar_start, size_t length) {
    PPToken pptoken = {
        .kind = kind,
        .splicedchar_start = splicedchar_start,
        .length = length,
    };

    return pptoken;
}

static PPToken tokenize_string_literal(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    size_t quote_count = 0;
    while (
        quote_count < 2
    ) {
        if (spliced_chars->data[i].value == '\"') quote_count++;
        if (spliced_chars->data[i].value == '\\') i++, length++;
        if (spliced_chars->data[i].value == '\n') panic("newline inside string literal");
        if (i + 1 == spliced_chars->count) panic("unterminated string literal");
        i++;
        length++;
    }

    return pptoken_create(PP_STRING, spliced_chars->data + start, length);
}

static PPToken tokenize_character_constant(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    size_t quote_count = 0;
    while (
        quote_count < 2
    ) {
        if (spliced_chars->data[i].value == '\'') quote_count++;
        if (spliced_chars->data[i].value == '\\') i++, length++;
        if (spliced_chars->data[i].value == '\n') panic("newline inside character constant");
        if (i + 1 == spliced_chars->count) panic("unterminated character constant");
        i++;
        length++;
    }

    return pptoken_create(PP_CHAR, spliced_chars->data + start, length);
}

static PPToken tokenize_block_comment(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        !(spliced_chars->data[i].value == '*' &&
          i + 1 < spliced_chars->count &&
          spliced_chars->data[i + 1].value == '/'
        )
    ) {
        if (i + 1 == spliced_chars->count) panic("unterminated block comment");
        length++;
        i++;
    }

    PPToken pptoken = pptoken_create(PP_WHITESPACE, spliced_chars->data + start, length + 2);
    return pptoken;
}

static PPToken tokenize_single_line_comment(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        // splicer guarantees newline at end of file
        spliced_chars->data[i++].value != '\n'
    ) {
        length++;
    }
    PPToken pptoken = pptoken_create(PP_WHITESPACE, spliced_chars->data + start, length);
    return pptoken;
}

static bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\v' || c == '\f';
}

static PPToken tokenize_whitespace(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        // splicer guarantees newline at end of file
        is_space(spliced_chars->data[i].value)
    ) {
        length++;
        i++;
    }

    PPToken pptoken = pptoken_create(PP_WHITESPACE, spliced_chars->data + start, length);
    return pptoken;
}

static bool is_ident_begin(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static PPToken tokenize_identifier(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        // splicer guarantees newline at end of file
        is_ident_begin(spliced_chars->data[i].value) ||
        is_digit(spliced_chars->data[i].value)
    ) {
        length++;
        i++;
    }

    PPToken pptoken = pptoken_create(PP_IDENTIFIER, spliced_chars->data + start, length);
    return pptoken;
}

static PPToken tokenize_number(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        // splicer guarantees newline at end of file
        is_digit(spliced_chars->data[i].value) ||
        is_ident_begin(spliced_chars->data[i].value) ||
        spliced_chars->data[i].value == '.'
    ) {
        if (
            (spliced_chars->data[i].value == 'E' ||
             spliced_chars->data[i].value == 'e' ||
             spliced_chars->data[i].value == 'P' ||
             spliced_chars->data[i].value == 'p'
            ) &&
            (i + 1 < spliced_chars->count &&
             (spliced_chars->data[i + 1].value == '+' ||
              spliced_chars->data[i + 1].value == '-'
             )
            )
        ) {
            i++;
            length++;
        }
        length++;
        i++;
    }

    PPToken pptoken = pptoken_create(PP_NUMBER, spliced_chars->data + start, length);
    return pptoken;
}

static bool is_include(PPToken pptoken) {
    char* compare = "include";
    if (pptoken.kind != PP_IDENTIFIER) return false;
    if (pptoken.length != sizeof(compare) - 1) return false;
    for (size_t i = 0; i < pptoken.length; ++i) {
        if (pptoken.splicedchar_start[i].value != compare[i]) return false;
    }
    return true;
}

static bool is_hash(PPToken pptoken) {
    if (pptoken.kind != PP_PUNCTUATOR) return false;
    if (pptoken.length != 1) return false;
    return pptoken.splicedchar_start->value == '#';
}

static bool check_hash_include(PPTokenVector* pptokens) {
    s64 i = pptokens->count - 1;

    // 1. Skip WS backwards to find "include"
    while (i >= 0 && pptokens->data[i].kind == PP_WHITESPACE) i--;
    if (i >= 0 && !is_include(pptokens->data[i])) return false;

    // 2. Skip WS backwards to find "#"
    i--;
    while (i >= 0 && pptokens->data[i].kind == PP_WHITESPACE) i--;
    if (i >= 0 && !is_hash(pptokens->data[i])) return false;

    // 3. We want to verify there is nothing BUT whitespace before the hash.
    i--;
    while (i >= 0 && pptokens->data[i].kind == PP_WHITESPACE) i--;

    // 4. Now check the boundary
    if (i < 0) return true;                                  // Start of file (Valid)
    if (pptokens->data[i].kind != PP_NEWLINE) return false;  // Found junk before hash (Invalid)

    return true;
}

static PPToken tokenize_header_name(SplicedCharVector* spliced_chars, size_t i) {
    size_t start = i;
    size_t length = 0;
    while (
        // splicer guarantees newline at end of file
        spliced_chars->data[i].value != '>'
    ) {
        if (spliced_chars->data[i].value == '\n') panic("newline inside header name");
        if (i + 1 == spliced_chars->count) panic("unterminated header name");
        i++;
        length++;
    }

    return pptoken_create(PP_HEADERNAME, spliced_chars->data + start, length + 1);
}

static PPToken tokenize_punctuator(SplicedCharVector* spliced_chars, size_t i) {
    // 1. Setup Helpers
    char a = spliced_chars->data[i].value;
    char b = (i + 1 < spliced_chars->count) ? spliced_chars->data[i + 1].value : '\0';
    char c = (i + 2 < spliced_chars->count) ? spliced_chars->data[i + 2].value : '\0';
    char d = (i + 3 < spliced_chars->count) ? spliced_chars->data[i + 3].value : '\0';  // Needed for %:%:

    switch (a) {
        // --- 1. The Singles (No multi-char versions) ---
        case '[':
        case ']':
        case '(':
        case ')':
        case '{':
        case '}':
        case '?':
        case ';':
        case '~':
        case ',':
            return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);

        // --- 2. Colon and C23 Double Colon ---
        case ':':
            if (b == '>')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // Digraph ']'
            else if (b == ':')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // C23 '::'
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // :

        // --- 3. Dot and Ellipsis ---
        case '.':
            if (b == '.' && c == '.')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 3);  // ...
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // .

        // --- 4. Plus Group ---
        case '+':
            if (b == '+')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // ++
            else if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // +=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // +

        // --- 5. Minus Group ---
        case '-':
            if (b == '>')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // ->
            else if (b == '-')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // --
            else if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // -=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // -

        // --- 6. Ampersand Group ---
        case '&':
            if (b == '&')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // &&
            else if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // &=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // &

        // --- 7. Pipe Group ---
        case '|':
            if (b == '|')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // ||
            else if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // |=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // |

        // --- 8. Asterisk Group ---
        case '*':
            if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // *=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // *

        // --- 9. Slash Group ---
        case '/':
            // Note: Comments are handled BEFORE this function.
            if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // /=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // /

        // --- 10. Bang Group ---
        case '!':
            if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // !=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // !

        // --- 11. Equals Group ---
        case '=':
            if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // ==
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // =

        // --- 12. Caret Group ---
        case '^':
            if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // ^=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // ^

        // --- 13. Hash Group ---
        case '#':
            if (b == '#')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // ##
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // #

        // --- 14. Less Than Group (+ Digraphs) ---
        case '<':
            if (b == '<') {
                if (c == '=')
                    return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 3);  // <<=
                else
                    return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // <<
            } else if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // <=
            else if (b == ':')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // <: (Digraph [)
            else if (b == '%')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // <% (Digraph {)
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // <

        // --- 15. Greater Than Group ---
        case '>':
            if (b == '>') {
                if (c == '=')
                    return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 3);  // >>=
                else
                    return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // >>
            } else if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // >=
            else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // >

        // --- 16. Percent Group (+ Digraphs) ---
        case '%':
            if (b == '=')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // %=
            else if (b == '>')
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // %> (Digraph })
            else if (b == ':') {
                // Check for %:%: (Digraph ##)
                if (c == '%' && d == ':')
                    return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 4);
                else
                    return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 2);  // %: (Digraph #)
            } else
                return pptoken_create(PP_PUNCTUATOR, spliced_chars->data + i, 1);  // %

        // --- Fallback ---
        default:
            return pptoken_create(PP_OTHER, spliced_chars->data + i, 1);
    }
}

PPTokenVector* tokenize(SplicedCharVector* spliced_chars) {
    PPTokenVector* pptokens = ARENA_ALLOC(PPTokenVector, 1);

    for (size_t i = 0; i < spliced_chars->count;) {
        PPToken pptoken;
        // "string literals"
        if ((spliced_chars->data[i].value == 'u' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '8' &&
             i + 2 < spliced_chars->count &&
             spliced_chars->data[i + 2].value == '\"'
            ) ||
            (spliced_chars->data[i].value == 'u' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\"'
            ) ||
            (spliced_chars->data[i].value == 'U' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\"'
            ) ||
            (spliced_chars->data[i].value == 'L' &&
             i + 1 < spliced_chars->count &&
             spliced_chars->data[i + 1].value == '\"'
            ) ||
            (spliced_chars->data[i].value == '\"'
            )) {
            pptoken = tokenize_string_literal(spliced_chars, i);
        }

        // 'character constants'
        else if ((spliced_chars->data[i].value == 'u' &&
                  i + 1 < spliced_chars->count &&
                  spliced_chars->data[i + 1].value == '8' &&
                  i + 2 < spliced_chars->count &&
                  spliced_chars->data[i + 2].value == '\''
                 ) ||
                 (spliced_chars->data[i].value == 'u' &&
                  i + 1 < spliced_chars->count &&
                  spliced_chars->data[i + 1].value == '\''
                 ) ||
                 (spliced_chars->data[i].value == 'U' &&
                  i + 1 < spliced_chars->count &&
                  spliced_chars->data[i + 1].value == '\''
                 ) ||
                 (spliced_chars->data[i].value == 'L' &&
                  i + 1 < spliced_chars->count &&
                  spliced_chars->data[i + 1].value == '\''
                 ) ||
                 (spliced_chars->data[i].value == '\''
                 )) {
            pptoken = tokenize_character_constant(spliced_chars, i);
        }

        /* BLOCK COMMENTS */
        else if (
            spliced_chars->data[i].value == '/' &&
            i + 1 < spliced_chars->count &&
            spliced_chars->data[i + 1].value == '*'
        ) {
            pptoken = tokenize_block_comment(spliced_chars, i);
        }

        // single line comments
        else if (
            spliced_chars->data[i].value == '/' &&
            i + 1 < spliced_chars->count &&
            spliced_chars->data[i + 1].value == '/'
        ) {
            pptoken = tokenize_single_line_comment(spliced_chars, i);
        }

        // newlines
        else if (
            spliced_chars->data[i].value == '\n'
        ) {
            pptoken = pptoken_create(PP_NEWLINE, spliced_chars->data + i, 1);
        }

        // whitespace
        else if (
            is_space(spliced_chars->data[i].value)
        ) {
            pptoken = tokenize_whitespace(spliced_chars, i);
        }

        // identifiers
        else if (
            is_ident_begin(spliced_chars->data[i].value)
        ) {
            pptoken = tokenize_identifier(spliced_chars, i);
        }

        // pp numbers
        else if (
            is_digit(spliced_chars->data[i].value) ||
            (spliced_chars->data[i].value == '.' &&
             i + 1 < spliced_chars->count &&
             is_digit(spliced_chars->data[i + 1].value)
            )
        ) {
            pptoken = tokenize_number(spliced_chars, i);
        }

        // header names
        else if (
            spliced_chars->data[i].value == '<' &&
            check_hash_include(pptokens)
        ) {
            pptoken = tokenize_header_name(spliced_chars, i);
        }

        // punctuators and others
        else {
            pptoken = tokenize_punctuator(spliced_chars, i);
        }

        vector_push(pptokens, pptoken);
        i += pptoken.length;
    }

    return pptokens;
}

void pptoken_print(PPToken pptoken) {
    char* ANSI_RESET = "\x1b[0m";
    char* ANSI_FG_BRIGHT[] = {
        "\x1b[90m",  // 0: Bright Black (Gray)
        "\x1b[91m",  // 1: Bright Red
        "\x1b[92m",  // 2: Bright Green
        "\x1b[93m",  // 3: Bright Yellow
        "\x1b[94m",  // 4: Bright Blue
        "\x1b[95m",  // 5: Bright Magenta
        "\x1b[96m",  // 6: Bright Cyan
        "\x1b[97m",  // 7: Bright White
    };
    puts(ANSI_FG_BRIGHT[pptoken.kind % 8]);
    for (size_t i = 0; i < pptoken.length; ++i) {
        putc(pptoken.splicedchar_start[i].source_char->byte->value);
    }
    puts(ANSI_RESET);
}
