#ifndef FILE_H
#define FILE_H

#include "token.h"
#include "types.h"

typedef struct File {
    // I originate from
    Token* org_tok;

    // this is my name
    char* name;

    // and this is my original data
    u8* og_data;  // raw bytes
    size_t og_size;

    // This is currently my data
    // as i evolve
    char* curr_data;
} File;

File* file_read(char* filename, Token* org_tok);

#endif  //  FILE_H
