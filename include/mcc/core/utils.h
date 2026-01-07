#ifndef MCC_CORE_UTILS_H
#define MCC_CORE_UTILS_H

#define MCC_CORE_UTILS_WORD_SIZE sizeof(void *)

#define MCC_CORE_UTILS_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MCC_CORE_UTILS_KiB(num) ((size_t)(num) << (10))
#define MCC_CORE_UTILS_MiB(num) ((size_t)(num) << (20))

#endif  // MCC_CORE_UTILS_H
