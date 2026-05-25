#include <stdio.h>
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    int status = 0;
    ck_hashset *set;

    set = ck_hashset_init(sizeof(uint64_t), ck_eq_u64, NULL);

    uint64_t present = 42;
    uint64_t missing = 7;
    if (ck_hashset_contains(set, &present)) {
        fprintf(stderr, "empty hashset should not contain value\n");
        status = 1;
        goto cleanup;
    }

    ck_hashset_insert(set, &present);
    if (!ck_hashset_contains(set, &present)) {
        fprintf(stderr, "hashset should contain inserted value\n");
        status = 1;
        goto cleanup;
    }

    if (ck_hashset_contains(set, &missing)) {
        fprintf(stderr, "hashset should not contain missing value\n");
        status = 1;
        goto cleanup;
    }

    for (uint64_t i = 0; i < 256; i++) {
        ck_hashset_insert(set, &i);
    }

    if (!ck_hashset_contains(set, &missing)) {
        fprintf(stderr, "hashset should find value after growth\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashset_deinit(set);
    return status;
}
