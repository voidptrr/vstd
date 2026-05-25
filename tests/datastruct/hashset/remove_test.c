#include <stdio.h>
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    int status = 0;
    ck_hashset *set;

    set = ck_hashset_init(sizeof(uint64_t), ck_eq_u64, NULL);

    for (uint64_t i = 0; i < 256; i++) {
        ck_hashset_insert(set, &i);
    }

    uint64_t removed = 42;
    ck_hashset_remove(set, &removed);
    if (ck_hashset_contains(set, &removed) || ck_hashset_size(set) != 255) {
        fprintf(stderr, "hashset remove should delete existing value\n");
        status = 1;
        goto cleanup;
    }

    ck_hashset_remove(set, &removed);
    if (ck_hashset_size(set) != 255) {
        fprintf(stderr, "hashset remove should ignore missing value\n");
        status = 1;
        goto cleanup;
    }

    uint64_t first = 0;
    uint64_t last = 255;
    ck_hashset_remove(set, &first);
    ck_hashset_remove(set, &last);
    if (ck_hashset_contains(set, &first) || ck_hashset_contains(set, &last) ||
        ck_hashset_size(set) != 253) {
        fprintf(stderr, "hashset remove should handle bucket head and chain nodes\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashset_deinit(set);
    return status;
}
