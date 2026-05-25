#include <stdio.h>

#include "ckit/datastruct/string.h"

int main(void) {
    int status = 0;
    ck_string value = ck_string_init("hello world", NULL);

    if (!ck_string_contains(value, "lo wo")) {
        fprintf(stderr, "contains should find substring\n");
        status = 1;
        goto cleanup;
    }

    if (ck_string_contains(value, "missing")) {
        fprintf(stderr, "contains should reject missing substring\n");
        status = 1;
        goto cleanup;
    }

    if (!ck_string_starts_with(value, "hello") || ck_string_starts_with(value, "world")) {
        fprintf(stderr, "starts_with should check prefix\n");
        status = 1;
        goto cleanup;
    }

    if (!ck_string_ends_with(value, "world") || ck_string_ends_with(value, "hello")) {
        fprintf(stderr, "ends_with should check suffix\n");
        status = 1;
        goto cleanup;
    }

    if (!ck_string_contains(value, "") || !ck_string_starts_with(value, "") ||
        !ck_string_ends_with(value, "")) {
        fprintf(stderr, "empty needle/prefix/suffix should match\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_string_deinit(value);
    return status;
}
