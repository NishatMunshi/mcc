// tokenizer_torture.c

// ---------------------------------------------------------
// TEST 1: The "Greedy Dot" (Section 6.4.8)
// ---------------------------------------------------------
/* C23 Definition: pp-number includes '.' 
   Therefore, a pp-number consumes dots greedily.
   
   Input:   1...
   Result:  [PP_NUMBER "1..."] 
   (Yes, this is a single valid token. It fails Phase 7 parsing, but passes Phase 3).
*/
1...


// ---------------------------------------------------------
// TEST 2: The "Greedy Exponent" (Section 6.4.8)
// ---------------------------------------------------------
/*
   Input:   1.0e+f
   Result:  [PP_NUMBER "1.0e+f"]
   
   Reasoning:
   1. "1.0" matches number.
   2. "e+" matches exponent part.
   3. "f" is a valid identifier-continue (hex digit or just letter).
   4. It gets eaten into the number.
*/
1.0e+f


// ---------------------------------------------------------
// TEST 3: The "Broken Exponent" 
// ---------------------------------------------------------
/*
   Input:   1.0e+
   Result:  [PP_NUMBER "1.0e+"]
   
   Reasoning:
   The tokenizer consumes 'e' and '+' as a pair. 
   EOF or whitespace stops it immediately after.
*/
1.0e+


// ---------------------------------------------------------
// TEST 4: The "Digit Separator" Trap
// ---------------------------------------------------------
/*
   Input:   123''456
   Result:  [PP_NUMBER "123"] [PUNCT "'"] [PUNCT "'"] [PP_NUMBER "456"]
   
   Reasoning:
   1. "123" is consumed.
   2. First ' is seen. 
   3. Lookahead sees second '. Is it digit/nondigit? NO.
   4. ' is NOT consumed. Token "123" ends.
   5. First ' becomes a punctuator (or char constant start).
*/
123''456


// ---------------------------------------------------------
// TEST 5: The "Identifier Normalization" (Section 6.4.2.1)
// ---------------------------------------------------------
/*
   Input:   caf\u00E9
   Result:  [PP_IDENTIFIER "café"]
   
   Reasoning:
   Your tokenizer MUST decode \u00E9 to UTF-8 bytes (0xC3 0xA9).
   The spelling of this token must be identical to the raw "café".
*/
int caf\u00E9 = 1;
int café = 1; // These should be the same identifier hash.


// ---------------------------------------------------------
// TEST 6: The "Operator Smash"
// ---------------------------------------------------------
/*
   Input:   x+++++y
   Result:  [ID "x"] [PUNCT "++"] [PUNCT "++"] [PUNCT "+"] [ID "y"]
*/
x+++++y


// ---------------------------------------------------------
// TEST 7: The "Header Name" Context
// ---------------------------------------------------------
/*
   Input:   #include <foo.h>
   Result:  [PUNCT "#"] [ID "include"] [HEADER_NAME "<foo.h>"]
*/
#include <foo.h>

/*
   Input:   x <foo.h>
   Result:  [ID "x"] [PUNCT "<"] [ID "foo"] [PUNCT "."] [ID "h"] [PUNCT ">"]
*/
x <foo.h>


// ---------------------------------------------------------
// TEST 8: The "Crash Me" (Constraint Violation)
// ---------------------------------------------------------
/*
   Input:   \u0061
   Result:  PANIC (or Error)
   
   Reasoning:
   0x61 is 'a'. It is in the Basic Character Set.
   Using it as a UCN is forbidden. Your parse_UCN should fail.
*/
