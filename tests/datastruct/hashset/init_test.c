#include <stdio.h>
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    int status = 0;
    ck_hashset *set;

    set = ck_hashset_init(sizeof(uint64_t), ck_eq_u64, NULL);
    if (ck_hashset_size(set) != 0) {
        fprintf(stderr, "new hashset should be empty\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashset_deinit(set);
    return status;
}
