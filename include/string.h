#ifndef _STRING_H
#define _STRING_H

#include "types.h"

size_t strlen(char* cstr);
bool streq(char* cstr1, char* cstr2);
void memcpy(void* dest, void* src, size_t num_bytes);

#endif  // _STRING_H
