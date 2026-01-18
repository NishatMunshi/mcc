#include <arena.h>
#include <panic.h>
#include <tokenizer.h>
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

/*
Encodes a vector of spliced chars (UTF-32) into
a single C-standard null terminated UTF-8 string
*/
static char* encode_UTF8(SplicedCharVector* spliced_chars) {
    // aggresive buffer (each u32 becoming 4 bytes)
    char* buf = ARENA_ALLOC(char, 4 * spliced_chars->count + 1);

    size_t buf_index = 0;
    for (size_t i = 0; i < spliced_chars->count; ++i) {
        u32 codepoint = spliced_chars->data[i]->value;

        if (codepoint <= 0x7f) {
            buf[buf_index++] = (char)codepoint;
        }

        else if (codepoint <= 0x7ff) {
            buf[buf_index++] = (char)(0xC0 | (codepoint >> 6));
            buf[buf_index++] = (char)(0x80 | (codepoint & 0x3F));
        }

        else if (codepoint <= 0xffff) {
            buf[buf_index++] = (char)(0xE0 | (codepoint >> 12));
            buf[buf_index++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
            buf[buf_index++] = (char)(0x80 | (codepoint & 0x3F));
        }

        else if (codepoint <= 0x10ffff) {
            buf[buf_index++] = (char)(0xF0 | (codepoint >> 18));
            buf[buf_index++] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
            buf[buf_index++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
            buf[buf_index++] = (char)(0x80 | (codepoint & 0x3F));
        }

        else {
            panic("invalid UTF-32 code point");
        }
    }

    buf[buf_index++] = 0;
    return buf;
}

static PPToken* pptoken_create(PPTokenKind kind, SplicedCharVector* origin) {
    PPToken* pptoken = ARENA_ALLOC(PPToken, 1);
    pptoken->kind = kind;
    pptoken->spelling = encode_UTF8(origin);
    pptoken->length = strlen(pptoken->spelling);
    pptoken->origin = origin;

    return pptoken;
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
            stream_consume(stream, 2);
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

    while(true) {
        SplicedChar* a = stream_peekahead(stream, 0);
        SplicedChar* b = stream_peekahead(stream, 1);

        if(a->value == '*' && b->value == '/') {
            vector_push(origin, a);
            vector_push(origin, b);
            stream_consume(stream, 2);
            break;
        }

        else if(a->value == 0) {
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

    while(true) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if(spliced_char->value == '\n') {
            break;
        }

        else if(spliced_char->value == 0) {
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

static bool is_space(u32 codepoint) {
    return codepoint == ' ' || codepoint == '\t' || codepoint == '\v' || codepoint == '\f';
}

static PPToken* tokenize_whitespace(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    while(true) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if(!is_space(spliced_char->value)) {
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

static bool is_ident_nondigit(u32 c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
}

static bool is_digit(u32 c) {
    return (c >= '0' && c <= '9');
}

static bool is_ident(u32 codepoint) {
    return is_ident_nondigit(codepoint) || is_digit(codepoint);
}

static PPToken* tokenize_identifier(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    while(true) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if(!is_ident(spliced_char->value)) {
            break;
        }

        else {
            vector_push(origin, spliced_char);
            stream_consume(stream, 1);
            continue;
        }
    }

    return pptoken_create(PP_IDENTIFIER, origin);
}

static bool is_pp_number(u32 codepoint) {
    return is_ident(codepoint) || codepoint == '.';
}

static PPToken* tokenize_number(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    while(true) {
        SplicedChar* curr_ptr = stream_peekahead(stream, 0);
        SplicedChar* next_ptr = stream_peekahead(stream, 1);

        u32 curr = curr_ptr->value;
        u32 next = next_ptr->value;

        if(curr == 0) {
            break;
        }

        if ((curr == 'e' || curr == 'E' || curr == 'p' || curr == 'P') && 
            (next == '+' || next == '-')) {
            vector_push(origin, curr_ptr);
            vector_push(origin, next_ptr);
            stream_consume(stream, 2);
            continue;
        }

        if (curr == '\'' && is_pp_number(next)) {
            vector_push(origin, curr_ptr);
            vector_push(origin, next_ptr);
            stream_consume(stream, 2);
            continue;
        }

        if (is_pp_number(curr)) {
            vector_push(origin, curr_ptr);
            stream_consume(stream, 1);
            continue;
        }

        break;
    }

    return pptoken_create(PP_NUMBER, origin);
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
    if (i < 0) return true;                                 // Start of file (Valid)
    if (pptokens->data[i]->kind != PP_NEWLINE) return false;  // Found junk before hash (Invalid)

    return true;
}

static PPToken* tokenize_header_name(SplicedCharStream* stream) {
    SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);

    while(true) {
        SplicedChar* spliced_char = stream_peekahead(stream, 0);

        if(spliced_char->value == '>') {
            break;
        }

        else if(spliced_char->value == 0) {
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
        case '[': case ']': case '(': case ')': 
        case '{': case '}': case '?': case ';': 
        case '~': case ',':
            len = 1;
            break;

        // --- 2. Colon group ---
        case ':':
            if (b == '>') len = 2;      // :> (Digraph ])
            else if (b == ':') len = 2; // :: (C23)
            else len = 1;
            break;

        // --- 3. Dot group ---
        case '.':
            if (b == '.' && c == '.') len = 3; // ...
            else len = 1;
            break;

        // --- 4. Plus group ---
        case '+':
            if (b == '+' || b == '=') len = 2;
            else len = 1;
            break;

        // --- 5. Minus group ---
        case '-':
            if (b == '>' || b == '-' || b == '=') len = 2;
            else len = 1;
            break;

        // --- 6. Ampersand group ---
        case '&':
            if (b == '&' || b == '=') len = 2;
            else len = 1;
            break;

        // --- 7. Pipe group ---
        case '|':
            if (b == '|' || b == '=') len = 2;
            else len = 1;
            break;

        // --- 8. Asterisk/Bang/Equals/Caret/Slash ---
        case '*': case '!': case '=': case '^': case '/':
            if (b == '=') len = 2;
            else len = 1;
            break;

        // --- 9. Hash group ---
        case '#':
            if (b == '#') len = 2; // ##
            else len = 1;
            break;

        // --- 10. Less Than group ---
        case '<':
            if (b == '<') {
                if (c == '=') len = 3; // <<=
                else len = 2;          // <<
            }
            else if (b == '=' || b == ':' || b == '%') len = 2; // <= or <: or <%
            else len = 1;
            break;

        // --- 11. Greater Than group ---
        case '>':
            if (b == '>') {
                if (c == '=') len = 3; // >>=
                else len = 2;          // >>
            }
            else if (b == '=') len = 2; // >=
            else len = 1;
            break;

        // --- 12. Percent group ---
        case '%':
            if (b == '=') len = 2;      // %=
            else if (b == '>') len = 2; // %>
            else if (b == ':') {
                if (c == '%' && d == ':') len = 4; // %:%: (Digraph ##)
                else len = 2;                      // %:   (Digraph #)
            }
            else len = 1;
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

        if(cp0 == 0) {
            break;
        }

        PPToken* pptoken = nullptr;

        // "string literals"
        if ((cp0 == 'u' && cp1 == '8' && cp2 == '\"') ||
            (cp0 == 'u' && cp1 == '\"') ||
            (cp0 == 'U' && cp1 == '\"') ||
            (cp0 == 'L' && cp1 == '\"')) {
            pptoken = tokenize_string_literal(&stream);
        }

        // 'character constants'
        else if ((cp0 == 'u' && cp1 == '8' && cp2 == '\'') ||
            (cp0 == 'u' && cp1 == '\'') ||
            (cp0 == 'U' && cp1 == '\'') ||
            (cp0 == 'L' && cp1 == '\'')) {
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
            SplicedCharVector* origin = ARENA_ALLOC(SplicedCharVector, 1);
            vector_push(origin, sc0);
            pptoken = pptoken_create(PP_NEWLINE, origin);
        }

        // whitespace
        else if (is_space(cp0)) {
            pptoken = tokenize_whitespace(&stream);
        }

        // identifiers
        else if (is_ident_nondigit(cp0)) {
            pptoken = tokenize_identifier(&stream);
        }

        // pp numbers
        else if (is_digit(cp0) || (cp0 == '.' && is_digit(cp1))) {
            pptoken = tokenize_number(&stream);
        }

        // header names
        else if (cp0 == '<' && check_hash_include(pptokens)) {
            pptoken = tokenize_header_name(&stream);
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