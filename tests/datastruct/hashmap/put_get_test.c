#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    ckit_hashmap *map;
    uint64_t key = 7U;
    uint64_t value = 11U;
    uint64_t value2 = 99U;
    const uint64_t *out;

    map = ckit_hashmap_init(sizeof(uint64_t), sizeof(uint64_t), ckit_eq_u64, NULL);

    if (ckit_hashmap_get(map, &key) != NULL) {
        fprintf(stderr, "missing key should return NULL\n");
        ckit_hashmap_free(map);
        return 1;
    }

    ckit_hashmap_put(map, &key, &value);
    out = (const uint64_t *)ckit_hashmap_get(map, &key);
    if (out == NULL || *out != value) {
        fprintf(stderr, "get should return inserted value\n");
        ckit_hashmap_free(map);
        return 1;
    }

    ckit_hashmap_put(map, &key, &value2);
    out = (const uint64_t *)ckit_hashmap_get(map, &key);
    if (out == NULL || *out != value2 || ckit_hashmap_size(map) != 1U) {
        fprintf(stderr, "put should update existing key in place\n");
        ckit_hashmap_free(map);
        return 1;
    }

    ckit_hashmap_free(map);
    return 0;
}
