#include <stdio.h>
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    int status = 0;
    ck_hashset *set;

    set = ck_hashset_init(sizeof(uint64_t), ck_eq_u64, NULL);

    uint64_t first = 42;
    ck_hashset_insert(set, &first);
    ck_hashset_insert(set, &first);
    if (ck_hashset_size(set) != 1) {
        fprintf(stderr, "hashset insert should ignore duplicates\n");
        status = 1;
        goto cleanup;
    }

    for (uint64_t i = 0; i < 256; i++) {
        ck_hashset_insert(set, &i);
    }

    if (ck_hashset_size(set) != 256) {
        fprintf(stderr, "hashset size should match unique inserted values\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashset_deinit(set);
    return status;
}
