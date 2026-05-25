#include <stdint.h>
#include <stdio.h>

#include "ckit/memory/allocators/arena.h"
#include "ckit/memory/utils.h"

int main(void) {
    ckit_arena *arena = ckit_arena_init(256);
    uint64_t *value;
    uint64_t *grown;
    size_t grown_size = CKIT_MEMORY_ALIGN * 2;

    value = (uint64_t *)ckit_arena_alloc(arena, sizeof(*value));
    if (value == NULL) {
        fprintf(stderr, "arena alloc should return memory\n");
        ckit_arena_deinit(arena);
        return 1;
    }
    *value = 42;

    grown = (uint64_t *)ckit_arena_realloc(arena, value, grown_size);
    if (grown == NULL || grown == value || grown[0] != 42) {
        fprintf(stderr, "arena realloc should grow and preserve old bytes\n");
        ckit_arena_deinit(arena);
        return 1;
    }

    if (ckit_arena_realloc(arena, grown, grown_size) != grown) {
        fprintf(stderr, "arena realloc with same size should return same pointer\n");
        ckit_arena_deinit(arena);
        return 1;
    }

    ckit_arena_deinit(arena);
    return 0;
}
