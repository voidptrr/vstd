#include <stdio.h>
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    ckit_hashset *set;

    set = ckit_hashset_init(sizeof(uint64_t), ckit_eq_u64, NULL);
    if (ckit_hashset_size(set) != 0) {
        fprintf(stderr, "new hashset should be empty\n");
        ckit_hashset_deinit(set);
        return 1;
    }

    ckit_hashset_deinit(set);
    return 0;
}
