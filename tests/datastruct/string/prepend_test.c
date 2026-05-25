#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    ckit_string value = ckit_string_init("world", NULL);

    ckit_string_prepend(&value, "hello ");
    if (strcmp(value, "hello world") != 0) {
        fprintf(stderr, "prepend should add prefix before existing string\n");
        ckit_string_deinit(value);
        return 1;
    }

    ckit_string_prepend(&value, "");
    if (strcmp(value, "hello world") != 0) {
        fprintf(stderr, "empty prepend should not change string\n");
        ckit_string_deinit(value);
        return 1;
    }

    ckit_string_deinit(value);
    return 0;
}
