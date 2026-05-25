#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    int status = 0;
    ck_string value = ck_string_init("hello", NULL);

    ck_string_append(&value, ", ");
    ck_string_append(&value, "world");
    ck_string_append(&value, "");
    if (ck_string_len(value) != 12 || strcmp(value, "hello, world") != 0) {
        fprintf(stderr, "string should append short suffixes\n");
        status = 1;
        goto cleanup;
    }

    ck_string_append(&value, "012345678901234567890123456789");
    if (ck_string_len(value) != 42 ||
        strcmp(value, "hello, world012345678901234567890123456789") != 0) {
        fprintf(stderr, "string should append across capacity growth\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_string_deinit(value);
    return status;
}
