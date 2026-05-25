#ifndef CK_MEMORY_BYTES_H
#define CK_MEMORY_BYTES_H

#include <stddef.h>

/* Swap size bytes between memory regions a and b. */
void ck_memswap(void *a, void *b, size_t size);

#endif
