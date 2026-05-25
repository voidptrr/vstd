#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    ckit_hashmap *map;

    map = ckit_hashmap_init(sizeof(uint64_t), sizeof(uint64_t), ckit_eq_u64, NULL);

    for (uint64_t i = 0; i < 256; i++) {
        uint64_t value = i * 3;
        ckit_hashmap_put(map, &i, &value);
    }

    if (ckit_hashmap_size(map) != 256) {
        fprintf(stderr, "hashmap size should match inserted count\n");
        ckit_hashmap_free(map);
        return 1;
    }

    {
        uint64_t key = 128;
        ckit_hashmap_remove(map, &key);
        if (ckit_hashmap_get(map, &key) != NULL) {
            fprintf(stderr, "removed key should not be found\n");
            ckit_hashmap_free(map);
            return 1;
        }
    }

    ckit_hashmap_free(map);
    return 0;
}
