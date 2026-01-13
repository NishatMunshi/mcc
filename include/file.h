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

    // This is my data after normalizing
    char* norm_data;
    size_t norm_size;

    // This is my data after splicing
    char* spl_data;
    size_t spl_size;

    // This is the line map
    // line_map[logic_line] == phys_line
    size_t* line_map;
    size_t line_map_size;
} File;

File* file_read(char* filename, Token* org_tok);

#endif  //  FILE_H
