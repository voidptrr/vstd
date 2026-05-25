#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    int status = 0;
    ck_hashmap *map;

    map = ck_hashmap_init(sizeof(uint64_t), sizeof(uint64_t), ck_eq_u64, NULL);
    if (ck_hashmap_size(map) != 0) {
        fprintf(stderr, "new hashmap should be empty\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashmap_deinit(map);
    return status;
}
