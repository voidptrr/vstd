#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    ckit_hashset *set;
    const ckit_hashset *const_set;
    uint64_t value = 42;
    uint64_t missing = 7;
    uint64_t *found;
    const uint64_t *const_found;

    set = ckit_hashset_init(sizeof(uint64_t), ckit_eq_u64, NULL);

    if (ckit_hashset_get(set, &value) != NULL) {
        fprintf(stderr, "missing element should return NULL\n");
        ckit_hashset_free(set);
        return 1;
    }

    ckit_hashset_insert(set, &value);

    found = (uint64_t *)ckit_hashset_get(set, &value);
    if (found == NULL || *found != value) {
        fprintf(stderr, "get should return inserted element\n");
        ckit_hashset_free(set);
        return 1;
    }

    const_set = set;
    const_found = (const uint64_t *)ckit_hashset_get_const(const_set, &value);
    if (const_found == NULL || *const_found != value) {
        fprintf(stderr, "get_const should return inserted element\n");
        ckit_hashset_free(set);
        return 1;
    }

    if (ckit_hashset_get(set, &missing) != NULL) {
        fprintf(stderr, "get should return NULL for missing element\n");
        ckit_hashset_free(set);
        return 1;
    }

    ckit_hashset_free(set);
    return 0;
}
