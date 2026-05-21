#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "ckit/compare.h"

int main(void) {
    uint8_t a[] = {1U, 2U, 3U};
    uint8_t b[] = {1U, 2U, 3U};
    uint8_t c[] = {1U, 2U, 4U};

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
    if (!ckit_eq_cstr(s1, s2, 0U) || ckit_eq_cstr(s1, s3, 0U)) {
        fprintf(stderr, "ckit_eq_cstr should compare strings\n");
        return 1;
    }

    int32_t i32a = -4;
    int32_t i32b = -4;
    int32_t i32c = 9;
    if (!ckit_eq_i32(&i32a, &i32b, 0U) || ckit_eq_i32(&i32a, &i32c, 0U)) {
        fprintf(stderr, "ckit_eq_i32 should compare int32 values\n");
        return 1;
    }

    uint64_t u64a = 42U;
    uint64_t u64b = 42U;
    uint64_t u64c = 77U;
    if (!ckit_eq_u64(&u64a, &u64b, 0U) || ckit_eq_u64(&u64a, &u64c, 0U)) {
        fprintf(stderr, "ckit_eq_u64 should compare uint64 values\n");
        return 1;
    }

    return 0;
}
