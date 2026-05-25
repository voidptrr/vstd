#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    int status = 0;
    ck_string empty = ck_string_init(NULL, NULL);
    if (empty == NULL || ck_string_len(empty) != 0 || strcmp(empty, "") != 0) {
        fprintf(stderr, "empty string should initialize to an empty C string\n");
        status = 1;
        goto cleanup_empty;
    }

cleanup_empty:
    ck_string_deinit(empty);
    if (status != 0) {
        return status;
    }

    ck_string value = ck_string_init("hello", NULL);
    if (value == NULL || ck_string_len(value) != 5 || strcmp(value, "hello") != 0) {
        fprintf(stderr, "string should initialize with contents\n");
        status = 1;
        goto cleanup_value;
    }

cleanup_value:
    ck_string_deinit(value);
    return status;
}
