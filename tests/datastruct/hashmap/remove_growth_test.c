#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    int status = 0;
    ck_hashmap *map;

    map = ck_hashmap_init(sizeof(uint64_t), sizeof(uint64_t), ck_eq_u64, NULL);

    for (uint64_t i = 0; i < 256; i++) {
        uint64_t value = i * 3;
        ck_hashmap_put(map, &i, &value);
    }

    if (ck_hashmap_size(map) != 256) {
        fprintf(stderr, "hashmap size should match inserted count\n");
        status = 1;
        goto cleanup;
    }

    {
        uint64_t key = 128;
        ck_hashmap_remove(map, &key);
        if (ck_hashmap_get(map, &key) != NULL) {
            fprintf(stderr, "removed key should not be found\n");
            status = 1;
            goto cleanup;
        }
    }

cleanup:
    ck_hashmap_deinit(map);
    return status;
}
