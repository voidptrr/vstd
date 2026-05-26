#ifndef CK_INTERNAL_MEMORY_UTILS_H
#define CK_INTERNAL_MEMORY_UTILS_H

#include <stddef.h>
#include <stdint.h>

#include "ckit/utils.h"

#define CK_MEMORY_ALIGN (sizeof(max_align_t))

/*
 * Round value up to the next multiple of alignment.
 * alignment must be a power of two and non-zero.
 */
static inline size_t ck_align_up(size_t value, size_t alignment) {
    size_t mask = alignment - 1;
    return (value + mask) & ~mask;
}

#endif
