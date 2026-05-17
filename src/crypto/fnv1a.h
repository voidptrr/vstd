#ifndef CSTD_CRYPTO_FNV1A_H
#define CSTD_CRYPTO_FNV1A_H

#include <stddef.h>
#include <stdint.h>

/* Internal FNV-1a 64-bit hash over raw bytes. */
static inline size_t cstd_internal_fnv1a_hash(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 14695981039346656037ULL;

    for (size_t i = 0; i < len; i++) {
        hash ^= (uint64_t)bytes[i];
        hash *= 1099511628211ULL;
    }

    return (size_t)hash;
}

#endif
