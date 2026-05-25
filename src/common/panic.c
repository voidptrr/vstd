#include <stdio.h>
#include <stdlib.h>

#include "ckit/common/panic.h"

_Noreturn void ck_panic(const char *message) {
    if (message != NULL) {
        fprintf(stderr, "%s\n", message);
    }
    abort();
}
