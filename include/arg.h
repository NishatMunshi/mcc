#ifndef ARG_H
#define ARG_H

#include "types.h"

typedef u8* va_list;

#define _STACK_SLOT_SIZE (sizeof(void*))

// Point 'ap' to the address immediately following 'last'
#define va_start(ap, last) \
    ((ap) = (u8*)&(last) + _STACK_SLOT_SIZE)

// 1. Advance the pointer
// 2. Cast the previous location to the type and dereference
#define va_arg(ap, type)       \
    ((ap) += _STACK_SLOT_SIZE, \
     *(type*)((ap) - _STACK_SLOT_SIZE))

// Cleanup is a no-op
#define va_end(ap) ((void)0)

#endif  // ARG_H
