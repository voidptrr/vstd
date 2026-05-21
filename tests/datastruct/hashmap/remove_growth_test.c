#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    ckit_hashmap map;

    ckit_hashmap_init(&map, sizeof(uint64_t), sizeof(uint64_t), ckit_eq_u64, NULL);

    for (uint64_t i = 0U; i < 256U; i++) {
        uint64_t value = i * 3U;
        ckit_hashmap_put(&map, &i, &value);
    }

    if (ckit_hashmap_size(&map) != 256U) {
        fprintf(stderr, "hashmap size should match inserted count\n");
        ckit_hashmap_free(&map);
        return 1;
    }

    {
        uint64_t key = 128U;
        uint64_t *removed = (uint64_t *)ckit_hashmap_remove(&map, &key);
        if (removed == NULL || *removed != key * 3U) {
            fprintf(stderr, "remove should return removed value\n");
            ckit_hashmap_free(&map);
            return 1;
        }
        if (ckit_hashmap_get(&map, &key) != NULL) {
            fprintf(stderr, "removed key should not be found\n");
            free(removed);
            ckit_hashmap_free(&map);
            return 1;
        }

        free(removed);
    }

    ckit_hashmap_free(&map);
    return 0;
}
