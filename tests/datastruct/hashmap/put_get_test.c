#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    int status = 0;
    ck_hashmap *map;
    uint64_t key = 7;
    uint64_t value = 11;
    uint64_t value2 = 99;
    const uint64_t *out;
    const ck_hashmap *const_map;

    map = ck_hashmap_init(sizeof(uint64_t), sizeof(uint64_t), ck_eq_u64, NULL);

    if (ck_hashmap_get(map, &key) != NULL) {
        fprintf(stderr, "missing key should return NULL\n");
        status = 1;
        goto cleanup;
    }

    ck_hashmap_put(map, &key, &value);
    out = (uint64_t *)ck_hashmap_get(map, &key);
    if (out == NULL || *out != value) {
        fprintf(stderr, "get should return inserted value\n");
        status = 1;
        goto cleanup;
    }

    const_map = map;
    out = (const uint64_t *)ck_hashmap_get_const(const_map, &key);
    if (out == NULL || *out != value) {
        fprintf(stderr, "get_const should return inserted value\n");
        status = 1;
        goto cleanup;
    }

    ck_hashmap_put(map, &key, &value2);
    out = (uint64_t *)ck_hashmap_get(map, &key);
    if (out == NULL || *out != value2 || ck_hashmap_size(map) != 1) {
        fprintf(stderr, "put should update existing key in place\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashmap_deinit(map);
    return status;
}
