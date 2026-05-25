#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    ckit_string value = ckit_string_init("hello", NULL);

    ckit_string_clear(value);
    if (ckit_string_len(value) != 0 || strcmp(value, "") != 0) {
        fprintf(stderr, "clear should reset string to empty\n");
        ckit_string_deinit(value);
        return 1;
    }

    ckit_string_append(&value, "again");
    if (strcmp(value, "again") != 0) {
        fprintf(stderr, "append should work after clear\n");
        ckit_string_deinit(value);
        return 1;
    }

    ckit_string_deinit(value);
    return 0;
}
