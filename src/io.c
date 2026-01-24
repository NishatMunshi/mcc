#include <arg.h>
#include <io.h>
#include <linux.h>
#include <string.h>

static void print_char(s32 fd, char c) {
    linux_write(fd, &c, 1);
}

static void print_string(s32 fd, char* cstr) {
    while (*(cstr) != '\0') {
        print_char(fd, *cstr);
        cstr++;
    }
}

static void print_u64(s32 fd, u64 num, u64 base) {
    if (num == 0) {
        print_char(fd, '0');
        return;
    }

    // 2^64 - 1 has 20 digits, or 16 hexits
    // 32 is the next power of two after 21
    char buf[32];
    char digits[] = "0123456789abcdef";

    // fill the buffer backwards
    size_t i = 0;
    while (num != 0) {
        buf[i] = digits[num % base];
        num = num / base;
        i++;
    }

    // now i = number of digits / hexits
    while (i > 0) {
        print_char(fd, buf[i - 1]);
        i--;
    }

    return;
}

void __fprintf_impl(
    s64 rdi,
    s64 rsi,
    s64 rdx,
    s64 rcx,
    s64 r8,
    s64 r9,
    s32 fd,
    char* format,
    ...
) {
    (void)rdi;
    (void)rsi;
    (void)rdx;
    (void)rcx;
    (void)r8;
    (void)r9;

    va_list ap;
    va_start(ap, format);

    size_t format_length = strlen(format);
    for (size_t i = 0; i < format_length;) {
        if (format[i] != '%') {
            print_char(fd, format[i]);
            i++;
            continue;
        }

        // now we are sure format[i] == '%'
        // one byte lookahead is always safe
        // because of null termination

        // character
        if (format[i + 1] == 'c') {
            print_char(fd, va_arg(ap, char));
            i += 2;
            continue;
        }

        // string
        if (format[i + 1] == 's') {
            print_string(fd, va_arg(ap, char*));
            i += 2;
            continue;
        }

        // 64 bit pointer
        if (format[i + 1] == 'x') {
            print_string(fd, "0x");
            print_u64(fd, va_arg(ap, u64), 16);
            i += 2;
            continue;
        }

        // the literal '%' symbol
        if (format[i + 1] == '%') {
            print_char(fd, '%');
            i += 2;
            continue;
        }

        // 64 bit decimal unsigned integer
        if (
            format[i + 1] == 'z' &&
            i + 1 < strlen(format) &&
            format[i + 2] == 'u'
        ) {
            print_u64(fd, va_arg(ap, u64), 10);
            i += 3;
            continue;
        }

        // if we fall here, we don't recognise the format specifier
        print_char(fd, format[i]);
        i++;
        continue;
    }

    va_end(ap);
}
