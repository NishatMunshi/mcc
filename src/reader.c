#include <arena.h>
#include <io.h>
#include <linux.h>
#include <panic.h>
#include <reader.h>
#include <string.h>
#include <vector.h>

typedef struct FileDefinitionMap {
    FileDefinition** data;
    size_t count;
    size_t capacity;
} FileDefinitionMap;

static FileDefinitionMap g_file_definitions = {0};

static FileDefinition* is_open(char* full_path) {
    for (size_t i = 0; i < g_file_definitions.count; ++i) {
        FileDefinition* definition = g_file_definitions.data[i];
        if (streq(definition->full_path, full_path)) {
            return definition;
        }
    }

    return nullptr;
}

static FileDefinition* get_definition(char* full_path) {
    FileDefinition* definition = is_open(full_path);

    if (definition != nullptr) {
        return definition;
    }

    else {
        s32 fd = linux_open(full_path, LINUX_FILE_FLAG_READONLY, 0);
        if (fd < 0) panic("failed to open file");

        linux_stat_t stat;
        if (linux_fstat(fd, &stat) < 0) panic("failed to stat file");
        s64 size = stat.st_size;

        u8* buf = ARENA_ALLOC(u8, size + 1);
        s64 bytes_read = linux_read(fd, buf, size);
        if (bytes_read < 0) panic("failed to read file");
        if (bytes_read < size) panic("partial read of file");
        linux_close(fd);

        buf[size] = '\0';

        FileDefinition* new_definition = ARENA_ALLOC(FileDefinition, 1);
        new_definition->full_path = full_path;
        new_definition->content = buf;
        new_definition->size = size;

        vector_push(&g_file_definitions, new_definition);
        return new_definition;
    }
}

ByteVector* read(char* full_path, PPToken* inclusion_trigger) {
    FileDefinition* definition = get_definition(full_path);

    FileInclusion* inclusion = ARENA_ALLOC(FileInclusion, 1);
    inclusion->definition = definition;
    inclusion->inclusion_trigger = inclusion_trigger;

    ByteVector* bytes = ARENA_ALLOC(ByteVector, 1);
    for (size_t i = 0; i < definition->size; ++i) {
        Byte* byte = ARENA_ALLOC(Byte, 1);
        byte->value = definition->content[i];
        byte->origin = inclusion;
        byte->offset = i;

        vector_push(bytes, byte);
    }

    return bytes;
}
