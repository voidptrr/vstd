#include <stddef.h>
#include <stdio.h>

#include "datastruct/vector.h"

static int test_vector_free_null_pointer(void) {
    cstd_status status;

    status = cstd_vector_free(NULL);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_vector_free(NULL) should return CSTD_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_vector_free_null_buffer(void) {
    cstd_status status;
    cstd_vector v = {0};

    status = cstd_vector_free(&v);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_free(&v) with NULL buffer should return CSTD_OK\n");
        return 1;
    }

    return 0;
}

static int test_vector_free_valid_vector(void) {
    cstd_status status;
    cstd_vector v;

    status = cstd_vector_init(&v, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_init(&v, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_vector_free(&v);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_free(&v) should return CSTD_OK\n");
        return 1;
    }

    if (v.buffer != NULL || v.size != 0 || v.capacity != 0) {
        fprintf(stderr, "cstd_vector_free(&v) should reset buffer, size, and capacity\n");
        return 1;
    }

    status = cstd_vector_free(&v);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_free(&v) should be idempotent\n");
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
