#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    ckit_string value = ckit_string_init("hello", NULL);

    ckit_string_append(&value, ", ");
    ckit_string_append(&value, "world");
    ckit_string_append(&value, "");
    if (ckit_string_len(value) != 12U || strcmp(value, "hello, world") != 0) {
        fprintf(stderr, "string should append short suffixes\n");
        ckit_string_free(value);
        return 1;
    }

    ckit_string_append(&value, "012345678901234567890123456789");
    if (ckit_string_len(value) != 42U ||
        strcmp(value, "hello, world012345678901234567890123456789") != 0) {
        fprintf(stderr, "string should append across capacity growth\n");
        ckit_string_free(value);
        return 1;
    }

    ckit_string_free(value);
    return 0;
}
