#ifndef CKIT_MEM_UTILS_H
#define CKIT_MEM_UTILS_H

#include <stddef.h>

/*
 * Round value up to the next multiple of alignment.
 * alignment must be a power of two and non-zero.
 */
static inline size_t ckit_align_up(size_t value, size_t alignment) {
    size_t mask = alignment - 1U;
    return (value + mask) & ~mask;
}

#endif
