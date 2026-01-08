#include "mcc/core/file.h"

#include "mcc/core/error.h"

static long _mcc_core_file_get_len(FILE *file, char *file_name) {
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        mcc_core_error_fatal("mcc_core_file_read: could not seek the end of file '%s'", file_name);
    }

    long len = ftell(file);
    if (len == -1) {
        fclose(file);
        mcc_core_error_fatal("mcc_core_file_read: could not tell the size of file '%s'", file_name);
    }

    // restore the changes before return
    rewind(file);
    return len;
}

static FILE *_mcc_core_file_open_bin(char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        mcc_core_error_fatal("mcc_core_file_read: failed to open file '%s'", file_name);
    }

    return file;
}

mcc_core_string *mcc_core_file_read(mcc_core_arena *arena, mcc_core_string *file_name) {
     char *file_name_cstr = mcc_core_string_get_cstr(file_name);

    // binary read (we will memcpy into buffer of mcc_core_string)
    FILE *file = _mcc_core_file_open_bin(file_name_cstr);
    long len = _mcc_core_file_get_len(file, file_name_cstr);

    // +1 for null terminator
    char *buffer = MCC_CORE_ARENA_ALLOCATE(arena, char, len + 1);
    size_t read_count = fread(buffer, 1, len, file);
    if (read_count != (size_t)len) {
        fclose(file);
        mcc_core_error_fatal("mcc_core_file_read: could not read file '%s'", file_name_cstr);
    }
    buffer[len] = '\0';

    // safety
    fclose(file);

    return mcc_core_string_construct_move_from_cstr(arena, buffer, (size_t)len);
}