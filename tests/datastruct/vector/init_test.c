#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "datastruct/vector.h"

static int test_vector_init_null_pointer(void) {
    cstd_status status;

    status = cstd_vector_init(NULL, sizeof(int));
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_vector_init(NULL, ...) should return CSTD_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_vector_init_valid_pointer(void) {
    cstd_status status;
    cstd_vector v;

    status = cstd_vector_init(&v, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_init(&v, ...) should return CSTD_OK\n");
        return 1;
    }

    if (v.buffer == NULL || v.size != 0 || v.elem_size != sizeof(int) || v.capacity != 16) {
        fprintf(stderr, "cstd_vector_init(&v, ...) returned invalid vector state\n");
        free(v.buffer);
        return 1;
    }

    free(v.buffer);
    return 0;
}

int main(void) {
    if (test_vector_init_null_pointer() != 0) {
        return 1;
    }

    if (test_vector_init_valid_pointer() != 0) {
        return 1;
    }

    return 0;
}
