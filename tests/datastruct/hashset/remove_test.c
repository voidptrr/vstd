#include <stdio.h>
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    ckit_hashset *set;

    set = ckit_hashset_init(sizeof(uint64_t), ckit_eq_u64, NULL);

    for (uint64_t i = 0; i < 256; i++) {
        ckit_hashset_insert(set, &i);
    }

    uint64_t removed = 42;
    ckit_hashset_remove(set, &removed);
    if (ckit_hashset_contains(set, &removed) || ckit_hashset_size(set) != 255) {
        fprintf(stderr, "hashset remove should delete existing value\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    ckit_hashset_remove(set, &removed);
    if (ckit_hashset_size(set) != 255) {
        fprintf(stderr, "hashset remove should ignore missing value\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    uint64_t first = 0;
    uint64_t last = 255;
    ckit_hashset_remove(set, &first);
    ckit_hashset_remove(set, &last);
    if (ckit_hashset_contains(set, &first) || ckit_hashset_contains(set, &last) ||
        ckit_hashset_size(set) != 253) {
        fprintf(stderr, "hashset remove should handle bucket head and chain nodes\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    ckit_hashset_deinit(set);
    return 0;
}
