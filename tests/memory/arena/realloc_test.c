#include <stdint.h>
#include <stdio.h>

#include "ckit/memory/allocators/arena.h"
#include "memory/utils.h"

int main(void) {
    int status = 0;
    ck_arena *arena = ck_arena_init(256);
    uint64_t *value;
    uint64_t *grown;
    size_t grown_size = CK_MEMORY_ALIGN * 2;

    value = (uint64_t *)ck_arena_alloc(arena, sizeof(uint64_t));
    if (value == NULL) {
        fprintf(stderr, "arena alloc should return memory\n");
        status = 1;
        goto cleanup;
    }
    *value = 42;

    grown = (uint64_t *)ck_arena_realloc(arena, value, grown_size);
    if (grown == NULL || grown == value || grown[0] != 42) {
        fprintf(stderr, "arena realloc should grow and preserve old bytes\n");
        status = 1;
        goto cleanup;
    }

    if (ck_arena_realloc(arena, grown, grown_size) != grown) {
        fprintf(stderr, "arena realloc with same size should return same pointer\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_arena_deinit(arena);
    return status;
}
