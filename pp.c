# 1 "src/main.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3








# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "src/main.c" 2
# 1 "include/main.h" 1



# 1 "include/types.h" 1



typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned char u8;

typedef long long s64;
typedef int s32;

typedef u64 size_t;

typedef union {
    long long ll;
    long double ld;
} max_align_t;
# 5 "include/main.h" 2

s32 main(s32 argc, char** argv);
# 2 "src/main.c" 2

# 1 "include/arena.h" 1





void arena_init(void);
void* arena_alloc(size_t size);
size_t arena_usage_KiB(void);
# 4 "src/main.c" 2
# 1 "include/io.h" 1



# 1 "include/linux.h" 1
# 21 "include/linux.h"
[[noreturn]] void linux_exit(u8 code);
s64 linux_write(s32 fd, char* buf, size_t len);


u8* linux_brk(u8* ptr);

typedef struct {
    u64 st_dev;
    u64 st_ino;
    u64 st_nlink;
    u32 st_mode;
    u32 st_uid;
    u32 st_gid;
    u32 __pad0;
    u64 st_rdev;
    s64 st_size;
    s64 st_blksize;
    s64 st_blocks;
    s64 st_atime_sec;
    s64 st_atime_nsec;
    s64 st_mtime_sec;
    s64 st_mtime_nsec;
    s64 st_ctime_sec;
    s64 st_ctime_nsec;
    s64 __unused[3];
} linux_stat_t;

s32 linux_open(char* filename, s32 flags, s32 mode);
s64 linux_close(s32 fd);
s64 linux_read(s32 fd, u8* buf, size_t len);
s64 linux_fstat(s32 fd, linux_stat_t* stat);
# 5 "include/io.h" 2





s64 fputc(s32 stream, char c);
s64 fputu(s32 stream, u64 num);
s64 fputs(s32 stream, char* cstr);

s64 putc(char c);
s64 putu(u64 num);
s64 puts(char* cstr);
# 5 "src/main.c" 2

# 1 "include/panic.h" 1



[[noreturn]] void panic(char* message);
# 7 "src/main.c" 2
# 1 "include/reader.h" 1






typedef struct PPToken PPToken;



typedef struct FileDefinition {
    char* path;

    u8* content;
    size_t size;
} FileDefinition;


typedef struct FileInclusion {
    FileDefinition* definition;

    PPToken* trigger;
    struct FileInclusion* parent;
} FileInclusion;


typedef struct Byte {
    char value;

    FileInclusion* inclusion;
    size_t offset;
} Byte;


typedef struct ByteVector {
    Byte** data;

    size_t count;
    size_t capacity;
} ByteVector;

ByteVector* read(char* path, PPToken* trigger);
# 8 "src/main.c" 2

s32 main(s32 argc, char** argv) {
    if (argc < 2) panic("no input file");

    arena_init();

    ByteVector* bytes = read(argv[1], nullptr);

    for (size_t i = 0; i < bytes->count; ++i) {
        putc(bytes->data[i]->value);
    }

    size_t arena_usage = arena_usage_KiB();
    puts("arena use = ");
    putu(arena_usage);
    puts(" KiB\n");
    return ((u8)0);
}
