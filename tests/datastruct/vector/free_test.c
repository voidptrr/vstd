#include <stddef.h>
#include <stdio.h>

#include "ckit/datastruct/vector.h"

static int test_vector_free_null_pointer(void) {
    ckit_status status;

    status = ckit_vector_free(NULL);
    if (status != CKIT_ERR_NULL) {
        fprintf(stderr, "ckit_vector_free(NULL) should return CKIT_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_vector_free_null_buffer(void) {
    ckit_status status;
    ckit_vector v = {0};

    status = ckit_vector_free(&v);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_free(&v) with NULL buffer should return CKIT_OK\n");
        return 1;
    }

    return 0;
}

static int test_vector_free_valid_vector(void) {
    ckit_status status;
    ckit_vector v;

    status = ckit_vector_init(&v, sizeof(int), NULL);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_init(&v, ..., NULL) should return CKIT_OK\n");
        return 1;
    }

    status = ckit_vector_free(&v);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_free(&v) should return CKIT_OK\n");
        return 1;
    }

    if (v.buffer != NULL || v.size != 0 || v.capacity != 0) {
        fprintf(stderr, "ckit_vector_free(&v) should reset buffer, size, and capacity\n");
        return 1;
    }

    status = ckit_vector_free(&v);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_free(&v) should be idempotent\n");
        return 1;
    }

    return 0;
}

int main(void) {
    if (test_vector_free_null_pointer() != 0) {
        return 1;
    }

    if (test_vector_free_null_buffer() != 0) {
        return 1;
    }

    if (test_vector_free_valid_vector() != 0) {
        return 1;
    }

    return 0;
}
