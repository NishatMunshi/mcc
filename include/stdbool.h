#ifndef _STDBOOL_H
#define _STDBOOL_H

// 1. The Type
// C11 uses the built-in keyword '_Bool' for the boolean type.
// This macro makes it accessible as the friendly 'bool'.
#define bool _Bool

// 2. The Values
// must expand to the integer constant 1
#define true 1

// must expand to the integer constant 0
#define false 0

// 3. The Conformance Signal
// This macro signals to the preprocessor that stdbool.h has been included.
// It is required for strict compliance checks.
#define __bool_true_false_are_defined 1

#endif
