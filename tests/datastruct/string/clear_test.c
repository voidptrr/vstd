#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    int status = 0;
    ck_string value = ck_string_init("hello", NULL);

    ck_string_clear(value);
    if (ck_string_len(value) != 0 || strcmp(value, "") != 0) {
        fprintf(stderr, "clear should reset string to empty\n");
        status = 1;
        goto cleanup;
    }

    ck_string_append(&value, "again");
    if (strcmp(value, "again") != 0) {
        fprintf(stderr, "append should work after clear\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_string_deinit(value);
    return status;
}
