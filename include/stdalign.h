#ifndef _STDALIGN_H
#define _STDALIGN_H

// 1. The Alignment Specifier
// Expands to the C11 keyword _Alignas
#define alignas _Alignas

// 2. The Alignment Operator
// Expands to the C11 keyword _Alignof
#define alignof _Alignof

// 3. Conformance Signals
// Required to allow #ifdef checks
#define __alignas_is_defined 1
#define __alignof_is_defined 1

#endif
