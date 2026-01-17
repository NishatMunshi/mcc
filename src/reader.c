#include <arena.h>
#include <linux.h>
#include <panic.h>
#include <reader.h>
#include <string.h>
#include <vector.h>

typedef struct FileDefinitionMap {
    FileDefinition* data;
    size_t count;
    size_t capacity;
} FileDefinitionMap;

typedef struct FileInclusionVector {
    FileInclusion* data;
    size_t count;
    size_t capacity;
} FileInclusionVector;

static FileDefinitionMap g_file_definitions = {0};
static FileInclusionVector g_file_inclusions = {0};

static FileDefinition* is_open(char* full_path) {
    for (size_t i = 0; i < g_file_definitions.count; ++i) {
        FileDefinition* definition = g_file_definitions.data + i;
        if (streq(definition->full_path, full_path)) {
            return definition;
        }
    }
    return nullptr;
}

static FileDefinition* get_definition(char* full_path) {
    FileDefinition* definition = is_open(full_path);
    if(definition != nullptr) {
        return definition;
    }

    else {
        s32 fd = linux_open(full_path, LINUX_FILE_FLAG_READONLY, 0);
        if (fd < 0) panic("failed to open file");

        linux_stat_t stat;
        if (linux_fstat(fd, &stat) < 0) panic("failed to stat file");
        size_t size = stat.st_size;

        u8* buf = ARENA_ALLOC(u8, size + 1);
        if (linux_read(fd, buf, size) < 0) panic("failed to read file");
        linux_close(fd);

        buf[size] = '\0';

        FileDefinition new_definition = {
            .full_path = full_path,
            .content = buf,
            .size = size,
        };

        vector_push(&g_file_definitions, new_definition);
        return vector_back(g_file_definitions);
    }
}

ByteVector read(char* full_path, PPToken* pptok) {
    FileDefinition* definition = get_definition(full_path);

    FileInclusion inclusion = {
        .definition = definition,
        .pptok = pptok,
    };

    vector_push(&g_file_inclusions, inclusion);

    ByteVector bytes = {0};
    for (size_t i = 0; i < definition->size; ++i) {
        Byte byte = {
            .value = definition->content[i],
            .inclusion = vector_back(g_file_inclusions),
            .offset = i,
        };

        vector_push(&bytes, byte);
    }

    return bytes;
}
