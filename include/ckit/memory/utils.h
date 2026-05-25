#ifndef CK_MEMORY_UTILS_H
#define CK_MEMORY_UTILS_H

#include <stddef.h>
#include <stdint.h>

#define CK_CONTAINER_OF(ptr, type, member) ((type *)((uint8_t *)(ptr) - offsetof(type, member)))
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
