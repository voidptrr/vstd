#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    int status = 0;
    ck_hashset *set;
    const ck_hashset *const_set;
    uint64_t value = 42;
    uint64_t missing = 7;
    uint64_t *found;
    const uint64_t *const_found;

    set = ck_hashset_init(sizeof(uint64_t), ck_eq_u64, NULL);

    if (ck_hashset_get(set, &value) != NULL) {
        fprintf(stderr, "missing element should return NULL\n");
        status = 1;
        goto cleanup;
    }

    ck_hashset_insert(set, &value);

    found = (uint64_t *)ck_hashset_get(set, &value);
    if (found == NULL || *found != value) {
        fprintf(stderr, "get should return inserted element\n");
        status = 1;
        goto cleanup;
    }

    const_set = set;
    const_found = (const uint64_t *)ck_hashset_get_const(const_set, &value);
    if (const_found == NULL || *const_found != value) {
        fprintf(stderr, "get_const should return inserted element\n");
        status = 1;
        goto cleanup;
    }

    if (ck_hashset_get(set, &missing) != NULL) {
        fprintf(stderr, "get should return NULL for missing element\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashset_deinit(set);
    return status;
}
