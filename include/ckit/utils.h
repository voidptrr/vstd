#ifndef CK_UTILS_H
#define CK_UTILS_H

#include <stddef.h>
#include <stdint.h>

#define CK_CONTAINER_OF(ptr, type, member) ((type *)((uint8_t *)(ptr) - offsetof(type, member)))

#endif
