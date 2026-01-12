#ifndef FILE_H
#define FILE_H

#include "token.h"
#include "types.h"

typedef struct File {
    // I originate from
    Token* org_tok;

    // this is my name
    char* name;

    // and this is the data I contain
    u8* data;  // raw bytes
    size_t size;
} File;

File* file_read(char* filename, Token* org_tok);

#endif  //  FILE_H
