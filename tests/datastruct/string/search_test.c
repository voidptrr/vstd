#include <stdio.h>

#include "ckit/datastruct/string.h"

int main(void) {
    ckit_string value = ckit_string_init("hello world", NULL);

    if (!ckit_string_contains(value, "lo wo")) {
        fprintf(stderr, "contains should find substring\n");
        ckit_string_deinit(value);
        return 1;
    }

    if (ckit_string_contains(value, "missing")) {
        fprintf(stderr, "contains should reject missing substring\n");
        ckit_string_deinit(value);
        return 1;
    }

    if (!ckit_string_starts_with(value, "hello") || ckit_string_starts_with(value, "world")) {
        fprintf(stderr, "starts_with should check prefix\n");
        ckit_string_deinit(value);
        return 1;
    }

    if (!ckit_string_ends_with(value, "world") || ckit_string_ends_with(value, "hello")) {
        fprintf(stderr, "ends_with should check suffix\n");
        ckit_string_deinit(value);
        return 1;
    }

    if (!ckit_string_contains(value, "") || !ckit_string_starts_with(value, "") ||
        !ckit_string_ends_with(value, "")) {
        fprintf(stderr, "empty needle/prefix/suffix should match\n");
        ckit_string_deinit(value);
        return 1;
    }

    ckit_string_deinit(value);
    return 0;
}
