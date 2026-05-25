#include <stdio.h>
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    ckit_hashset *set;

    set = ckit_hashset_init(sizeof(uint64_t), ckit_eq_u64, NULL);

    uint64_t present = 42;
    uint64_t missing = 7;
    if (ckit_hashset_contains(set, &present)) {
        fprintf(stderr, "empty hashset should not contain value\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    ckit_hashset_insert(set, &present);
    if (!ckit_hashset_contains(set, &present)) {
        fprintf(stderr, "hashset should contain inserted value\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    if (ckit_hashset_contains(set, &missing)) {
        fprintf(stderr, "hashset should not contain missing value\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        ckit_hashset_insert(set, &i);
    }

    if (!ckit_hashset_contains(set, &missing)) {
        fprintf(stderr, "hashset should find value after growth\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    ckit_hashset_deinit(set);
    return 0;
}
