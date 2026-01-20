#ifndef UNICODE_H
#define UNICODE_H

/*
UTF-32 is chosen as the source character set
as per section 5.2.1 (Character sets) of the
C standard ISO/IEC 9899:2024.
*/

bool is_digit(u32 codepoint);
bool is_nondigit(u32 codepoint);
bool is_XID_Start(u32 codepoint);
bool is_XID_Continue(u32 codepoint);
bool is_inline_whitespace(u32 codepoint);
bool is_hex_digit(u32 codepoint);

#endif  // UNICODE_H
