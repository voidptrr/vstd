#include <stdint.h>

#include "ckit/memory/bytes.h"

void ck_memswap(void *a, void *b, size_t size) {
    uint8_t *left = (uint8_t *)a;
    uint8_t *right = (uint8_t *)b;

    for (size_t i = 0; i < size; i++) {
        uint8_t tmp = left[i];
        left[i] = right[i];
        right[i] = tmp;
    }
}
