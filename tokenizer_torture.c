/* * tokenizer_torture.c
 * A stress test for C23 Phase 3 Tokenization
 */

// =========================================================
// TEST 1: UCN Normalization & Equivalence (Section 6.4.2.1)
// =========================================================
/* The compiler must see these as identical identifiers.
   Your lexer should produce TOK_IDENTIFIER for both, and 
   their 'spelling' (semantic value) must be identical bytes.
*/
#define \u03C0 3.14159   // Defines PI
float val = π;           // Uses PI (Raw UTF-8)

// Mixed usage in one identifier (Valid in C23)
int f\u006Fo = 1;        // Spelling: "foo"
int \u0066oo = 2;        // Spelling: "foo" (Redeclaration error in parser, but same ID)


// =========================================================
// TEST 2: UCNs in String Literals (Phase 5 deferral)
// =========================================================
/* Your lexer MUST NOT decode these. 
   Spelling must preserve the 6 characters: \ u 0 0 6 1
*/
char* raw = "\u0061"; 


// =========================================================
// TEST 3: The "Greedy" PP-Number (Section 6.4.8)
// =========================================================
/* These are all SINGLE tokens (PP_NUMBER).
   Many are semantically invalid, but lexically valid.
*/

// Standard Float
0.123

// The "Too Many Dots" Edge Case
// Lexer must eat all of this as one token.
1.2.3.4e+5 

// Hex Float with Exponent
0x1.fp-2 

// Greedy Sign Consumption
// This should be ONE token: 1e+
// Because 'e' is followed by '+', it consumes it.
// The parser will choke on 'f', but the lexer must eat the '+'
1e+f 

// The Digit Separator (C23)
1'000'000    // Single Token


// =========================================================
// TEST 4: Digit Separator Edge Cases
// =========================================================

// Valid separator usage
0xDEAD'BEEF  // Single Token

// Trailing Quote Trap
// This is NOT a separator because it's not followed by a digit/nondigit.
// Should be: PP_NUMBER(123) followed by PUNCT(')
123' 


// =========================================================
// TEST 5: Header Names vs. Less Than
// =========================================================

// Context: #include
#include <stdio.h>      // Token: HEADER_NAME(<stdio.h>)

// Context: Normal code
// Should be: PUNCT(<) IDENTIFIER(stdio) PUNCT(.) IDENTIFIER(h) PUNCT(>)
int x = <stdio.h>; 


// =========================================================
// TEST 6: Maximal Munch (Operator Clashes)
// =========================================================

// "x plus plus plus plus plus y"
// Tokens: ID(x) PUNCT(++) PUNCT(++) PUNCT(+) ID(y)
x+++++y 

// "Right Shift Assign" vs "Right Shift" + "Assign"
// Token: PUNCT(>>=)
x>>=1

// Digraphs (C23 still supports them!)
// <: is [ and :> is ]
// Tokens: PUNCT([) PUNCT(])
<::>


// =========================================================
// TEST 7: C23 Basic Character Set Exceptions
// =========================================================
/* $, @, and ` are now Basic Characters.
   They cannot be UCN escaped.
   But they can be used raw if the implementation allows.
*/
int $amount = 0;   // Valid Identifier (usually extension, but lexically ID)
int @ignored = 0;  // Often used in Objective-C, but strictly usually invalid ID char
