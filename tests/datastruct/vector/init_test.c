#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "ckit/datastruct/vector.h"

static int test_vector_init_null_pointer(void) {
    ckit_status status;

    status = ckit_vector_init(NULL, sizeof(int), NULL);
    if (status != CKIT_ERR_NULL) {
        fprintf(stderr, "ckit_vector_init(NULL, ..., NULL) should return CKIT_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_vector_init_valid_pointer(void) {
    ckit_status status;
    ckit_vector v;

    status = ckit_vector_init(&v, sizeof(int), NULL);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_init(&v, ..., NULL) should return CKIT_OK\n");
        return 1;
    }

    if (v.buffer == NULL || v.size != 0 || v.elem_size != sizeof(int) || v.capacity != 16) {
        fprintf(stderr, "ckit_vector_init(&v, ..., NULL) returned invalid vector state\n");
        free(v.buffer);
        return 1;
    }

    free(v.buffer);
    return 0;
}

static int test_vector_init_zero_elem_size(void) {
    ckit_vector v;
    ckit_status status = ckit_vector_init(&v, 0, NULL);

    if (status != CKIT_ERR_RANGE) {
        fprintf(stderr, "ckit_vector_init(&v, 0, NULL) should return CKIT_ERR_RANGE\n");
        return 1;
    }

    return 0;
}

static int test_vector_is_empty(void) {
    ckit_vector v;
    int value = 1;

    if (ckit_vector_is_empty(NULL) != true) {
        fprintf(stderr, "ckit_vector_is_empty(NULL) should return true\n");
        return 1;
    }

    if (ckit_vector_init(&v, sizeof(int), NULL) != CKIT_OK) {
        fprintf(stderr, "ckit_vector_init(&v, ..., NULL) should return CKIT_OK\n");
        return 1;
    }

    if (ckit_vector_is_empty(&v) != true) {
        fprintf(stderr, "ckit_vector_is_empty(&v) should return true for empty vector\n");
        free(v.buffer);
        return 1;
    }

    if (ckit_vector_push(&v, &value) != CKIT_OK) {
        fprintf(stderr, "ckit_vector_push(&v, &value) should return CKIT_OK\n");
        free(v.buffer);
        return 1;
    }

    if (ckit_vector_is_empty(&v) != false) {
        fprintf(stderr, "ckit_vector_is_empty(&v) should return false after push\n");
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

    if (test_vector_init_zero_elem_size() != 0) {
        return 1;
    }

    if (test_vector_is_empty() != 0) {
        return 1;
    }

    return 0;
}
