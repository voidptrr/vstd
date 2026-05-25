#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    int status = 0;
    ck_vector *v;
    int value = 1;

    v = ck_vector_init(sizeof(int), NULL);
    if (ck_vector_size(v) != 0) {
        fprintf(stderr, "vector should initialize empty\n");
        status = 1;
        goto cleanup;
    }

    ck_vector_push(v, &value);
    if (ck_vector_size(v) != 1) {
        fprintf(stderr, "vector should contain one element after push\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_vector_deinit(v);
    return status;
}
