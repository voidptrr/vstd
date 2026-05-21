#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    ckit_hashmap map;

    ckit_hashmap_init(&map, sizeof(uint64_t), sizeof(uint64_t), ckit_eq_u64, NULL);
    if (ckit_hashmap_size(&map) != 0U || !ckit_hashmap_is_empty(&map)) {
        fprintf(stderr, "new hashmap should be empty\n");
        ckit_hashmap_free(&map);
        return 1;
    }

    ckit_hashmap_free(&map);
    return 0;
}
