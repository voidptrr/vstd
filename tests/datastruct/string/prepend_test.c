#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    int status = 0;
    ck_string value = ck_string_init("world", NULL);

    ck_string_prepend(&value, "hello ");
    if (strcmp(value, "hello world") != 0) {
        fprintf(stderr, "prepend should add prefix before existing string\n");
        status = 1;
        goto cleanup;
    }

    ck_string_prepend(&value, "");
    if (strcmp(value, "hello world") != 0) {
        fprintf(stderr, "empty prepend should not change string\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_string_deinit(value);
    return status;
}
