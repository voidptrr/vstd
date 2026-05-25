#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"

int main(void) {
    int32_t i32a = -2;
    int32_t i32b = 9;
    int32_t i32c = -2;

    if (!(ck_cmp_i32(&i32a, &i32b) < 0 && ck_cmp_i32(&i32b, &i32a) > 0 &&
          ck_cmp_i32(&i32a, &i32c) == 0)) {
        fprintf(stderr, "ck_cmp_i32 should provide strict weak ordering\n");
        return 1;
    }

    uint64_t u64a = 1;
    uint64_t u64b = 2;
    uint64_t u64c = 2;

    if (!(ck_cmp_u64(&u64a, &u64b) < 0 && ck_cmp_u64(&u64b, &u64a) > 0 &&
          ck_cmp_u64(&u64b, &u64c) == 0)) {
        fprintf(stderr, "ck_cmp_u64 should provide strict weak ordering\n");
        return 1;
    }

    size_t size_a = 1;
    size_t size_b = 2;
    size_t size_c = 2;

    if (!(ck_cmp_size(&size_a, &size_b) < 0 && ck_cmp_size(&size_b, &size_a) > 0 &&
          ck_cmp_size(&size_b, &size_c) == 0)) {
        fprintf(stderr, "ck_cmp_size should provide strict weak ordering\n");
        return 1;
    }

    return 0;
}
