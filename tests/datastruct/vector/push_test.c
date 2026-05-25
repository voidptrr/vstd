#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    int status = 0;
    ck_vector *v;

    v = ck_vector_init(sizeof(int), NULL);

    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        ck_vector_push(v, &value);
    }

    if (ck_vector_size(v) != 17) {
        fprintf(stderr, "vector should store all pushed elements\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_vector_deinit(v);
    return status;
}
