#include <arena.h>
#include <io.h>
#include <panic.h>
#include <tokenizer.h>
#include <unicode.h>
#include <vector.h>

typedef struct SplicedCharStream {
    SplicedCharVector* spliced_chars;
    size_t current_index;
} SplicedCharStream;

/*
Peeks ahead by offset number of spliced chars without crashing.
If request goes out of bounds, returns a valid pointer to
a dummy Spliced Char which lives in static memory.
*/
static SplicedChar* stream_peekahead(SplicedCharStream* stream, size_t offset) {
    static SplicedChar EOF_SENTINEL = {
        .value = 0,
        .source_char = nullptr,
    };

    size_t target_index = stream->current_index + offset;

    if (target_index >= stream->spliced_chars->count) {
        return &EOF_SENTINEL;
    }

    return stream->spliced_chars->data[target_index];
}

/*
Consumes a maximum of the specified number of Spliced Chars.
If consumption goes out of bounds, consumes everything left safely.
*/
static void stream_consume(SplicedCharStream* stream, size_t count) {
    if (stream->current_index + count >= stream->spliced_chars->count) {
        stream->current_index = stream->spliced_chars->count;
    }

    else {
        stream->current_index += count;
    }
}

static bool is_valid_UCN(u32 codepoint) {
    // 1. The Short Identifier Check (C23 Simplification)
    if (codepoint <= 0x009F) {
        return false;
    }

    // 2. The Surrogate Pair Check (Standard Constraint)
    if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
        return false;
    }

    // 3. The Max Value Check (Standard Constraint)
    if (codepoint > 0x10FFFF) {
        return false;
    }

    return true;
}

u8 unicode_to_hex_digit(u32 codepoint) {
    if (codepoint >= '0' && codepoint <= '9') {
        return codepoint - '0';
    }

    if (codepoint >= 'a' && codepoint <= 'f') {
        return codepoint - 'a' + 0xa;
    }

    if (codepoint >= 'A' && codepoint <= 'F') {
        return codepoint - 'A' + 0xA;
    }

    panic("invalid hex digit");
}

u16 parse_hex_quad(u32 cp0, u32 cp1, u32 cp2, u32 cp3) {
    u16 hex_digit_0 = unicode_to_hex_digit(cp0);
    u16 hex_digit_1 = unicode_to_hex_digit(cp1);
    u16 hex_digit_2 = unicode_to_hex_digit(cp2);
    u16 hex_digit_3 = unicode_to_hex_digit(cp3);

    return (hex_digit_0 << 12) | (hex_digit_1 << 8) | (hex_digit_2 << 4) | hex_digit_3;
}

static u32 peek_UCN(SplicedCharStream* stream) {
    // grab 10 characters
    u32 cp0 = stream_peekahead(stream, 0)->value;
    u32 cp1 = stream_peekahead(stream, 1)->value;
    u32 cp2 = stream_peekahead(stream, 2)->value;
    u32 cp3 = stream_peekahead(stream, 3)->value;
    u32 cp4 = stream_peekahead(stream, 4)->value;
    u32 cp5 = stream_peekahead(stream, 5)->value;
    u32 cp6 = stream_peekahead(stream, 6)->value;
    u32 cp7 = stream_peekahead(stream, 7)->value;
    u32 cp8 = stream_peekahead(stream, 8)->value;
    u32 cp9 = stream_peekahead(stream, 9)->value;

    if (cp0 != '\\') {
        panic("invalid UCN found");
    }

    u32 codepoint = 0;

    if (cp1 == 'u') {
        codepoint = parse_hex_quad(cp2, cp3, cp4, cp5);
    }

    else if (cp1 == 'U') {
        u16 first_half = parse_hex_quad(cp2, cp3, cp4, cp5);
        u16 second_half = parse_hex_quad(cp6, cp7, cp8, cp9);
        codepoint = ((u32)first_half << 16) | second_half;
    }

    if (!is_valid_UCN(codepoint)) {
        printf("here, %c%c%c%c%c%c\n", cp0, cp1, cp2, cp3, cp4, cp5);
        panic("invalid UCN found");
    }

    return codepoint;
}

typedef struct UTF8 {
    size_t len;
    u8 bytes[4];
} UTF8;

UTF8 UTF32_to_UTF8(u32 codepoint) {
    UTF8 utf8 = {0};

    if (codepoint <= 0x7f) {
        utf8.bytes[utf8.len++] = (char)codepoint;
    }

    else if (codepoint <= 0x7ff) {
        utf8.bytes[utf8.len++] = (char)(0xC0 | (codepoint >> 6));
        utf8.bytes[utf8.len++] = (char)(0x80 | (codepoint & 0x3F));
    }

    else if (codepoint <= 0xffff) {
        utf8.bytes[utf8.len++] = (char)(0xE0 | (codepoint >> 12));
        utf8.bytes[utf8.len++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        utf8.bytes[utf8.len++] = (char)(0x80 | (codepoint & 0x3F));
    }

    else if (codepoint <= 0x10ffff) {
        utf8.bytes[utf8.len++] = (char)(0xF0 | (codepoint >> 18));
        utf8.bytes[utf8.len++] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        utf8.bytes[utf8.len++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        utf8.bytes[utf8.len++] = (char)(0x80 | (codepoint & 0x3F));
    }

    else {
        panic("invalid UTF-32");
    }

    return utf8;
}

/*
Encodes a vector of spliced chars (UTF-32) into
a single C-standard null terminated UTF-8 string
*/
static char* encode_UTF8(SplicedCharVector* spliced_chars, bool should_encode_UCN) {
    // aggresive buffer (each UTF-32 becoming 4 bytes)
    char* buf = ARENA_ALLOC(char, 4 * spliced_chars->count + 1);
    SplicedCharStream stream = {
        .spliced_chars = spliced_chars,
        .current_index = 0,
    };

    size_t buf_index = 0;
    while (true) {
        u32 cp0 = stream_peekahead(&stream, 0)->value;
        u32 cp1 = stream_peekahead(&stream, 1)->value;

        if (cp0 == 0) {
            break;
        }

        /*
        If we encounter an UCN that needs encoding,
        we record the U32 of it, and record how many
        spliced chars to consume. Then pass it to
        the translator.
        */
        size_t to_consume = 1;
        if (should_encode_UCN && cp0 == '\\') {
            cp0 = peek_UCN(&stream);

            if (cp1 == 'u') {
                to_consume = 6;
            } else if (cp1 == 'U') {
                to_consume = 10;
            }
        }

        UTF8 utf8 = UTF32_to_UTF8(cp0);
        for (size_t i = 0; i < utf8.len; ++i) {
            buf[buf_index++] = utf8.bytes[i];
        }
        stream_consume(&stream, to_consume);
    }

    buf[buf_index] = 0;
    return buf;
}

static PPToken* pptoken_create(PPTokenKind kind, SplicedCharVector* origin) {
    PPToken* pptoken = ARENA_ALLOC(PPToken, 1);
    pptoken->kind = kind;

    bool should_encode_UCN = false;
    if (kind == PP_IDENTIFIER || kind == PP_NUMBER) {
        should_encode_UCN = true;
    }

    pptoken->spelling = encode_UTF8(origin, should_encode_UCN);
    pptoken->length = strlen(pptoken->spelling);
    pptoken->origin = origin;

    return pptoken;
}

static bool check_hash_include(PPTokenVector* pptokens) {
    s64 i = pptokens->count - 1;

    // 1. Skip WS backwards to find "include"
    while (i >= 0 && pptokens->data[i]->kind == PP_WHITESPACE) i--;
    if (i >= 0 && !pptoken_is(pptokens->data[i], PP_IDENTIFIER, "include")) return false;

    // 2. Skip WS backwards to find "#"
    i--;
    while (i >= 0 && pptokens->data[i]->kind == PP_WHITESPACE) i--;
    if (i >= 0 && !pptoken_is(pptokens->data[i], PP_PUNCTUATOR, "#")) return false;

    // 3. We want to verify there is nothing BUT whitespace before the hash.
    i--;
    while (i >= 0 && pptokens->data[i]->kind == PP_WHITESPACE) i--;

    // 4. Now check the boundary
    if (i < 0) return true;                                   // Start of file (Valid)
    if (pptokens->data[i]->kind != PP_NEWLINE) return false;  // Found junk before hash (Invalid)

    return true;
}

static PPToken* tokenize_header_name(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    u32 left_delim = stream_peekahead(stream, 0)->value;
    u32 right_delim = left_delim == '<' ? '>' : '\"';

    size_t delim_count = 0;
    while (true) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if (delim_count >= 2) {
            break;
        }

        if (spliced_char->value == left_delim ||
            spliced_char->value == right_delim) {
            delim_count++;
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
            continue;
        }

        else if (spliced_char->value == '\n') {
            panic("newline inside header name");
        }

        else if (spliced_char->value == 0) {
            panic("unterminated header name");
        }

        else {
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
            continue;
        }
    }

    return pptoken_create(PP_HEADERNAME, origin);
}

static PPToken* tokenize_string_literal(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    for (size_t quote_count = 0; quote_count < 2;) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if (spliced_char->value == '\"') {
            quote_count++;
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
        }

        else if (spliced_char->value == '\\') {
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
            vector_push(origin, stream_peekahead(stream, 0));
            stream_consume(stream, 1);
            continue;
        }

        else if (spliced_char->value == '\n') {
            panic("newline inside string literal");
        }

        else if (spliced_char->value == 0) {
            panic("unterminated string literal");
        }

        else {
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
        }
    }

    return pptoken_create(PP_STRING, origin);
}

static PPToken* tokenize_character_constant(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    for (size_t quote_count = 0; quote_count < 2;) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if (spliced_char->value == '\'') {
            quote_count++;
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
        }

        else if (spliced_char->value == '\\') {
            stream_consume(stream, 2);
        }

        else if (spliced_char->value == '\n') {
            panic("newline inside character constant");
        }

        else if (spliced_char->value == 0) {
            panic("unterminated character constant");
        }

        else {
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
        }
    }

    return pptoken_create(PP_CHAR, origin);
}

static PPToken* tokenize_block_comment(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    while (true) {
        SplicedChar* a = stream_peekahead(stream, 0);
        SplicedChar* b = stream_peekahead(stream, 1);

        if (a->value == '*' && b->value == '/') {
            vector_push(origin, a);
            vector_push(origin, b);
            stream_consume(stream, 2);
            break;
        }

        else if (a->value == 0) {
            panic("unterminated block comment");
        }

        else {
            vector_push(origin, a);
            stream_consume(stream, 1);
            continue;
        }
    }

    return pptoken_create(PP_WHITESPACE, origin);
}

static PPToken* tokenize_single_line_comment(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    while (true) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if (spliced_char->value == '\n') {
            break;
        }

        else if (spliced_char->value == 0) {
            panic("no newline at the end of single line comment");
        }

        else {
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
            continue;
        }
    }

    return pptoken_create(PP_WHITESPACE, origin);
}

static PPToken* tokenize_newline(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);
    SplicedChar* spliced_char = stream_peekahead(stream, 0);

    vector_push(origin, spliced_char);
    stream_consume(stream, 1);

    return pptoken_create(PP_NEWLINE, origin);
}

static PPToken* tokenize_whitespace(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    while (true) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if (!is_inline_whitespace(spliced_char->value)) {
            break;
        }

        else {
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
            continue;
        }
    }

    return pptoken_create(PP_WHITESPACE, origin);
}

static PPToken* tokenize_identifier(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    while (true) {
        SplicedChar* sc0 = stream_peekahead(stream, 0);
        u32 cp0 = sc0->value;

        if (is_digit(cp0) || is_nondigit(cp0) || is_XID_Continue(cp0)) {
            vector_push(origin, sc0);
            stream_consume(stream, 1);
            continue;
        }

        else if (cp0 == '\\' && is_XID_Continue(peek_UCN(stream))) {
            // consume the backslash and move on
            vector_push(origin, sc0);
            stream_consume(stream, 1);
            continue;
        }

        else {
            break;
        }
    }

    return pptoken_create(PP_IDENTIFIER, origin);
}

static PPToken* tokenize_pp_number(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    SplicedChar* sc_start = stream_peekahead(stream, 0);
    vector_push(origin, sc_start);
    stream_consume(stream, 1);

    while (true) {
        SplicedChar* sc0 = stream_peekahead(stream, 0);
        SplicedChar* sc1 = stream_peekahead(stream, 1);
        u32 cp0 = sc0->value;
        u32 cp1 = sc1->value;

        if ((cp0 == 'e' || cp0 == 'E' || cp0 == 'p' || cp0 == 'P') &&
            (cp1 == '+' || cp1 == '-')) {
            vector_push(origin, sc0);
            vector_push(origin, sc1);
            stream_consume(stream, 2);
            continue;
        }

        else if (cp0 == '.') {
            vector_push(origin, sc0);
            stream_consume(stream, 1);
            continue;
        }

        else if (cp0 == '\'') {
            if (is_digit(cp1) || is_nondigit(cp1) || is_XID_Continue(cp1)) {
                vector_push(origin, sc0);
                stream_consume(stream, 1);
                continue;
            } else {
                // Trailing quote is not part of number
                break;
            }
        }

        // D. Digits & Identifiers (covers normal 'e' without sign too)
        else if (is_digit(cp0) || is_nondigit(cp0) || is_XID_Continue(cp0)) {
            vector_push(origin, sc0);
            stream_consume(stream, 1);
            continue;
        }

        // E. UCNs
        // Same logic: eat backslash if UCN is valid ID char, let loop handle the rest
        else if (cp0 == '\\' && is_XID_Continue(peek_UCN(stream))) {
            vector_push(origin, sc0);
            stream_consume(stream, 1);
            continue;
        }

        else {
            break;
        }
    }

    return pptoken_create(PP_NUMBER, origin);
}

static PPToken* tokenize_punctuator(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    // Peek ahead to max possible punctuator length (4 for %:%:)
    SplicedChar* sc0 = stream_peekahead(stream, 0);
    SplicedChar* sc1 = stream_peekahead(stream, 1);
    SplicedChar* sc2 = stream_peekahead(stream, 2);
    SplicedChar* sc3 = stream_peekahead(stream, 3);

    u32 a = sc0->value;
    u32 b = sc1->value;
    u32 c = sc2->value;
    u32 d = sc3->value;

    size_t len = 1;
    PPTokenKind kind = PP_PUNCTUATOR;

    switch (a) {
        // --- 1. The Singles ---
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
            len = 1;
            break;

        // --- 2. Colon group ---
        case ':':
            if (b == '>')
                len = 2;  // :> (Digraph ])
            else if (b == ':')
                len = 2;  // :: (C23)
            else
                len = 1;
            break;

        // --- 3. Dot group ---
        case '.':
            if (b == '.' && c == '.')
                len = 3;  // ...
            else
                len = 1;
            break;

        // --- 4. Plus group ---
        case '+':
            if (b == '+' || b == '=')
                len = 2;
            else
                len = 1;
            break;

        // --- 5. Minus group ---
        case '-':
            if (b == '>' || b == '-' || b == '=')
                len = 2;
            else
                len = 1;
            break;

        // --- 6. Ampersand group ---
        case '&':
            if (b == '&' || b == '=')
                len = 2;
            else
                len = 1;
            break;

        // --- 7. Pipe group ---
        case '|':
            if (b == '|' || b == '=')
                len = 2;
            else
                len = 1;
            break;

        // --- 8. Asterisk/Bang/Equals/Caret/Slash ---
        case '*':
        case '!':
        case '=':
        case '^':
        case '/':
            if (b == '=')
                len = 2;
            else
                len = 1;
            break;

        // --- 9. Hash group ---
        case '#':
            if (b == '#')
                len = 2;  // ##
            else
                len = 1;
            break;

        // --- 10. Less Than group ---
        case '<':
            if (b == '<') {
                if (c == '=')
                    len = 3;  // <<=
                else
                    len = 2;  // <<
            } else if (b == '=' || b == ':' || b == '%')
                len = 2;  // <= or <: or <%
            else
                len = 1;
            break;

        // --- 11. Greater Than group ---
        case '>':
            if (b == '>') {
                if (c == '=')
                    len = 3;  // >>=
                else
                    len = 2;  // >>
            } else if (b == '=')
                len = 2;  // >=
            else
                len = 1;
            break;

        // --- 12. Percent group ---
        case '%':
            if (b == '=')
                len = 2;  // %=
            else if (b == '>')
                len = 2;  // %>
            else if (b == ':') {
                if (c == '%' && d == ':')
                    len = 4;  // %:%: (Digraph ##)
                else
                    len = 2;  // %:   (Digraph #)
            } else
                len = 1;
            break;

        // --- Fallback ---
        default:
            kind = PP_OTHER;
            len = 1;
            break;
    }

    // Common Construction Logic
    for (size_t i = 0; i < len; i++) {
        vector_push(origin, stream_peekahead(stream, i));
    }
    stream_consume(stream, len);

    return pptoken_create(kind, origin);
}

PPTokenVector* tokenize(SplicedCharVector* spliced_chars) {
    SplicedCharStream stream = {
        .spliced_chars = spliced_chars,
        .current_index = 0,
    };

    PPTokenVector* pptokens = ARENA_ALLOC(PPTokenVector, 1);
    while (true) {
        SplicedChar* sc0 = stream_peekahead(&stream, 0);
        SplicedChar* sc1 = stream_peekahead(&stream, 1);
        SplicedChar* sc2 = stream_peekahead(&stream, 2);

        u32 cp0 = sc0->value;
        u32 cp1 = sc1->value;
        u32 cp2 = sc2->value;

        if (cp0 == 0) {
            break;
        }

        PPToken* pptoken = nullptr;

        // header names
        if ((cp0 == '<' || cp0 == '\"') && check_hash_include(pptokens)) {
            pptoken = tokenize_header_name(&stream);
        }

        // "string literals"
        else if ((cp0 == 'u' && cp1 == '8' && cp2 == '\"') ||
                 (cp0 == 'u' && cp1 == '\"') ||
                 (cp0 == 'U' && cp1 == '\"') ||
                 (cp0 == 'L' && cp1 == '\"') ||
                 (cp0 == '\"')) {
            pptoken = tokenize_string_literal(&stream);
        }

        // 'character constants'
        else if ((cp0 == 'u' && cp1 == '8' && cp2 == '\'') ||
                 (cp0 == 'u' && cp1 == '\'') ||
                 (cp0 == 'U' && cp1 == '\'') ||
                 (cp0 == 'L' && cp1 == '\'') ||
                 (cp0 == '\'')) {
            pptoken = tokenize_character_constant(&stream);
        }

        /* BLOCK COMMENTS */
        else if (cp0 == '/' && cp1 == '*') {
            pptoken = tokenize_block_comment(&stream);
        }

        // single line comments
        else if (cp0 == '/' && cp1 == '/') {
            pptoken = tokenize_single_line_comment(&stream);
        }

        // newlines
        else if (cp0 == '\n') {
            pptoken = tokenize_newline(&stream);
        }

        // whitespace
        else if (is_inline_whitespace(cp0)) {
            pptoken = tokenize_whitespace(&stream);
        }

        // identifiers
        else if (cp0 == '\\' && (cp1 == 'u' || cp1 == 'U')) {
            u32 codepoint = peek_UCN(&stream);

            if (is_XID_Start(codepoint)) {
                pptoken = tokenize_identifier(&stream);
            }

            else {
                panic("invalid identifier found");
            }
        }

        else if (is_nondigit(cp0) || is_XID_Start(cp0)) {
            pptoken = tokenize_identifier(&stream);
        }

        // pp numbers
        else if (is_digit(cp0) || (cp0 == '.' && is_digit(cp1))) {
            pptoken = tokenize_pp_number(&stream);
        }

        // punctuators and others
        else {
            pptoken = tokenize_punctuator(&stream);
        }

        vector_push(pptokens, pptoken);
    }

    return pptokens;
}

bool pptoken_is(PPToken* pptoken, PPTokenKind kind, char* spelling) {
    if (pptoken->kind != kind) return false;
    return streq(pptoken->spelling, spelling);
}
