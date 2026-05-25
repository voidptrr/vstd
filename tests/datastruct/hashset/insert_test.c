#include <stdio.h>
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    ckit_hashset *set;

    set = ckit_hashset_init(sizeof(uint64_t), ckit_eq_u64, NULL);

    uint64_t first = 42;
    ckit_hashset_insert(set, &first);
    ckit_hashset_insert(set, &first);
    if (ckit_hashset_size(set) != 1) {
        fprintf(stderr, "hashset insert should ignore duplicates\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        ckit_hashset_insert(set, &i);
    }

    if (ckit_hashset_size(set) != 256) {
        fprintf(stderr, "hashset size should match unique inserted values\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    ckit_hashset_deinit(set);
    return 0;
}
