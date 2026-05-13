#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "datastruct/vector.h"

static int test_vector_push_appends_value(void) {
    cstd_status status;
    cstd_vector v;
    int value = 42;

    status = cstd_vector_init(&v, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_init(&v, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_vector_push(&v, &value);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_push(&v, &value) should return CSTD_OK\n");
        free(v.buffer);
        return 1;
    }

    if (v.size != 1) {
        fprintf(stderr, "vector_push should increment vector size\n");
        free(v.buffer);
        return 1;
    }

    if (((int *)v.buffer)[0] != value) {
        fprintf(stderr, "vector_push should copy pushed value into buffer\n");
        free(v.buffer);
        return 1;
    }

    free(v.buffer);
    return 0;
}

static int test_vector_push_grows_capacity(void) {
    cstd_status status;
    cstd_vector v;
    size_t initial_capacity;
    size_t i;

    status = cstd_vector_init(&v, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_init(&v, ...) should return CSTD_OK\n");
        return 1;
    }

    initial_capacity = v.capacity;

    for (i = 0; i < initial_capacity + 1; i++) {
        int value = (int)i;
        status = cstd_vector_push(&v, &value);
        if (status != CSTD_OK) {
            fprintf(stderr, "cstd_vector_push should succeed across capacity growth\n");
            free(v.buffer);
            return 1;
        }
    }

    if (v.capacity <= initial_capacity) {
        fprintf(stderr, "cstd_vector_push should grow capacity when full\n");
        free(v.buffer);
        return 1;
    }

    if (v.size != initial_capacity + 1) {
        fprintf(stderr, "cstd_vector_push should preserve all pushed elements\n");
        free(v.buffer);
        return 1;
    }

    if (((int *)v.buffer)[0] != 0) {
        fprintf(stderr, "cstd_vector_push should preserve existing values after realloc\n");
        free(v.buffer);
        return 1;
    }

    if (((int *)v.buffer)[initial_capacity] != (int)initial_capacity) {
        fprintf(stderr, "cstd_vector_push should keep correct value after realloc\n");
        free(v.buffer);
        return 1;
    }

    free(v.buffer);
    return 0;
}

static int test_vector_push_null_pointers(void) {
    cstd_status status;
    cstd_vector v;
    int value = 1;

    status = cstd_vector_push(NULL, &value);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_vector_push(NULL, &value) should return CSTD_ERR_NULL\n");
        return 1;
    }

    status = cstd_vector_init(&v, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_init(&v, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_vector_push(&v, NULL);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_vector_push(&v, NULL) should return CSTD_ERR_NULL\n");
        free(v.buffer);
        return 1;
    }

    free(v.buffer);
    return 0;
}

int main(void) {
    if (test_vector_push_appends_value() != 0) {
        return 1;
    }

    if (test_vector_push_grows_capacity() != 0) {
        return 1;
    }

    if (test_vector_push_null_pointers() != 0) {
        return 1;
    }

    return 0;
}
