#ifndef _STRING_H
#define _STRING_H

#include "types.h"

size_t strlen(char* cstr);
bool streq(char* cstr1, char* cstr2);
char* strcat(char* cstr1, char* cstr2);
char* strrchr(char* cstr, char c);
char* strdup(char* cstr);

void memcpy(void* dest, void* src, size_t num_bytes);
void* memset(void* ptr, int value, size_t num);

#endif  // _STRING_H
