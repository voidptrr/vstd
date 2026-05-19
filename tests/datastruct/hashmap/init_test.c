#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    ckit_hashmap map;

    if (ckit_hashmap_init(NULL, sizeof(uint64_t), sizeof(uint64_t), ckit_eq_u64, NULL) !=
        CKIT_ERR_NULL) {
        fprintf(stderr, "ckit_hashmap_init(NULL, ...) should return CKIT_ERR_NULL\n");
        return 1;
    }

    if (ckit_hashmap_init(&map, 0U, sizeof(uint64_t), ckit_eq_u64, NULL) != CKIT_ERR_RANGE) {
        fprintf(stderr, "ckit_hashmap_init key_size=0 should return CKIT_ERR_RANGE\n");
        return 1;
    }

    if (ckit_hashmap_init(&map, sizeof(uint64_t), 0U, ckit_eq_u64, NULL) != CKIT_ERR_RANGE) {
        fprintf(stderr, "ckit_hashmap_init value_size=0 should return CKIT_ERR_RANGE\n");
        return 1;
    }

    if (ckit_hashmap_init(&map, sizeof(uint64_t), sizeof(uint64_t), NULL, NULL) != CKIT_ERR_NULL) {
        fprintf(stderr, "ckit_hashmap_init NULL key_eq should return CKIT_ERR_NULL\n");
        return 1;
    }

    if (ckit_hashmap_init(&map, sizeof(uint64_t), sizeof(uint64_t), ckit_eq_u64, NULL) != CKIT_OK) {
        fprintf(stderr, "ckit_hashmap_init valid args should return CKIT_OK\n");
        return 1;
    }

    if (ckit_hashmap_size(&map) != 0U || !ckit_hashmap_is_empty(&map)) {
        fprintf(stderr, "new hashmap should be empty\n");
        ckit_hashmap_free(&map);
        return 1;
    }

    if (ckit_hashmap_free(&map) != CKIT_OK) {
        fprintf(stderr, "ckit_hashmap_free should return CKIT_OK\n");
        return 1;
    }

    return 0;
}
