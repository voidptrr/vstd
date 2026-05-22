#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"

int main(void) {
    uint8_t a[] = {1, 2, 3};
    uint8_t b[] = {1, 2, 3};
    uint8_t c[] = {1, 2, 4};

    if (!ckit_eq_bytes(a, b, sizeof(a))) {
        fprintf(stderr, "ckit_eq_bytes equal arrays should return true\n");
        return 1;
    }
    if (ckit_eq_bytes(a, c, sizeof(a))) {
        fprintf(stderr, "ckit_eq_bytes different arrays should return false\n");
        return 1;
    }

    const char *s1 = "hash";
    const char *s2 = "hash";
    const char *s3 = "map";
    if (!ckit_eq_cstr(s1, s2, 0) || ckit_eq_cstr(s1, s3, 0)) {
        fprintf(stderr, "ckit_eq_cstr should compare strings\n");
        return 1;
    }

    const char *sp1 = "hash";
    const char *sp2 = "hash";
    const char *sp3 = "map";
    if (!ckit_eq_cstr_ptr(&sp1, &sp2, 0) || ckit_eq_cstr_ptr(&sp1, &sp3, 0)) {
        fprintf(stderr, "ckit_eq_cstr_ptr should compare pointed-to strings\n");
        return 1;
    }

    int32_t i32a = -4;
    int32_t i32b = -4;
    int32_t i32c = 9;
    if (!ckit_eq_i32(&i32a, &i32b, 0) || ckit_eq_i32(&i32a, &i32c, 0)) {
        fprintf(stderr, "ckit_eq_i32 should compare int32 values\n");
        return 1;
    }

    uint64_t u64a = 42;
    uint64_t u64b = 42;
    uint64_t u64c = 77;
    if (!ckit_eq_u64(&u64a, &u64b, 0) || ckit_eq_u64(&u64a, &u64c, 0)) {
        fprintf(stderr, "ckit_eq_u64 should compare uint64 values\n");
        return 1;
    }

    size_t size_a = 42;
    size_t size_b = 42;
    size_t size_c = 77;
    if (!ckit_eq_size(&size_a, &size_b, 0) || ckit_eq_size(&size_a, &size_c, 0)) {
        fprintf(stderr, "ckit_eq_size should compare size_t values\n");
        return 1;
    }

    return 0;
}
