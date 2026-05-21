#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    ckit_string empty = ckit_string_init(NULL, NULL);
    if (empty == NULL || ckit_string_len(empty) != 0U || strcmp(empty, "") != 0) {
        fprintf(stderr, "empty string should initialize to an empty C string\n");
        ckit_string_free(empty);
        return 1;
    }
    ckit_string_free(empty);

    ckit_string value = ckit_string_init("hello", NULL);
    if (value == NULL || ckit_string_len(value) != 5U || strcmp(value, "hello") != 0) {
        fprintf(stderr, "string should initialize with contents\n");
        ckit_string_free(value);
        return 1;
    }

    ckit_string_free(value);
    return 0;
}
