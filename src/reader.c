#include <reader.h>

#include <arena.h>
#include <io.h>
#include <linux.h>
#include <panic.h>
#include <string.h>
#include <vector.h>

typedef struct FileDefinitionMap {
    FileDefinition** data;
    size_t count;
    size_t capacity;
} FileDefinitionMap;

static FileDefinitionMap g_file_definitions = {
    .data = nullptr,
    .count = 0,
    .capacity = 0,
};

static FileDefinition* is_open(char* path) {
    for (size_t i = 0; i < g_file_definitions.count; ++i) {
        FileDefinition* definition = g_file_definitions.data[i];
        if (streq(definition->path, path)) {
            return definition;
        }
    }
    return nullptr;
}

static FileDefinition* get_definition(char* path) {
    FileDefinition* definition = is_open(path);
    if (!definition) {
        definition = ARENA_ALLOC(FileDefinition, 1);

        s32 fd = linux_open(path, LINUX_FILE_FLAG_READONLY, 0);
        if (fd < 0) panic("failed to open file");
        
        linux_stat_t stat;
        if (linux_fstat(fd, &stat) < 0) panic("failed to stat file");
        size_t size = stat.st_size;

        u8* buf = ARENA_ALLOC(u8, size + 1);
        if (linux_read(fd, buf, size) < 0) panic("failed to read file");
        linux_close(fd);

        buf[size] = '\0';

        definition->path = path;
        definition->content = buf;
        definition->size = size;

        vector_push(&g_file_definitions, definition);
    }

    return definition;
}

ByteVector read(char* path, PPToken* pptok) {
    FileDefinition* definition = get_definition(path);

    FileInclusion* inclusion = ARENA_ALLOC(FileInclusion, 1);
    inclusion->definition = definition;
    inclusion->pptok = pptok;

    ByteVector bytes = {0};
    for (size_t i = 0; i < definition->size; ++i) {
        Byte byte = {
            .value = definition->content[i],
            .inclusion = inclusion,
            .offset = i,
        };

        vector_push(&bytes, byte);
    }

    return bytes;
}
