#include <stddef.h>
#include <stdio.h>

#include "datastruct/vector.h"

static int test_vector_pop_null_vector(void) {
    int out = 0;

    if (cstd_vector_pop(NULL, &out) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_vector_pop(NULL, &out) should return CSTD_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_vector_pop_null_out(void) {
    cstd_status status;
    cstd_vector v;

    status = cstd_vector_init(&v, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_init(&v, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_vector_pop(&v, NULL);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_vector_pop(&v, NULL) should return CSTD_ERR_NULL\n");
        cstd_vector_free(&v);
        return 1;
    }

    if (cstd_vector_free(&v) != CSTD_OK) {
        fprintf(stderr, "cstd_vector_free(&v) should return CSTD_OK\n");
        return 1;
    }

    return 0;
}

static int test_vector_pop_empty_vector(void) {
    cstd_status status;
    cstd_vector v;

    status = cstd_vector_init(&v, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_init(&v, ...) should return CSTD_OK\n");
        return 1;
    }

    {
        int out = 0;
        if (cstd_vector_pop(&v, &out) != CSTD_ERR_EMPTY) {
            fprintf(stderr, "cstd_vector_pop on empty vector should return CSTD_ERR_EMPTY\n");
            cstd_vector_free(&v);
            return 1;
        }
    }

    if (cstd_vector_size(&v) != 0) {
        fprintf(stderr, "cstd_vector_pop on empty vector should not change size\n");
        cstd_vector_free(&v);
        return 1;
    }

    if (cstd_vector_free(&v) != CSTD_OK) {
        fprintf(stderr, "cstd_vector_free(&v) should return CSTD_OK\n");
        return 1;
    }

    return 0;
}

static int test_vector_pop_returns_last_value(void) {
    cstd_status status;
    cstd_vector v;
    int first = 7;
    int second = 11;
    int popped = 0;

    status = cstd_vector_init(&v, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_init(&v, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_vector_push(&v, &first);
    if (status != CSTD_OK) {
        fprintf(stderr, "first cstd_vector_push should return CSTD_OK\n");
        cstd_vector_free(&v);
        return 1;
    }

    status = cstd_vector_push(&v, &second);
    if (status != CSTD_OK) {
        fprintf(stderr, "second cstd_vector_push should return CSTD_OK\n");
        cstd_vector_free(&v);
        return 1;
    }

    status = cstd_vector_pop(&v, &popped);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_vector_pop should return CSTD_OK\n");
        cstd_vector_free(&v);
        return 1;
    }

    if (popped != second) {
        fprintf(stderr, "cstd_vector_pop should return last pushed value\n");
        cstd_vector_free(&v);
        return 1;
    }

    if (cstd_vector_size(&v) != 1) {
        fprintf(stderr, "cstd_vector_pop should decrement vector size\n");
        cstd_vector_free(&v);
        return 1;
    }

    status = cstd_vector_pop(&v, &popped);
    if (status != CSTD_OK || popped != first) {
        fprintf(stderr, "cstd_vector_pop should preserve LIFO order\n");
        cstd_vector_free(&v);
        return 1;
    }

    if (cstd_vector_size(&v) != 0) {
        fprintf(stderr, "cstd_vector should be empty after popping all elements\n");
        cstd_vector_free(&v);
        return 1;
    }

    if (cstd_vector_free(&v) != CSTD_OK) {
        fprintf(stderr, "cstd_vector_free(&v) should return CSTD_OK\n");
        return 1;
    }

    return 0;
}

int main(void) {
    if (test_vector_pop_null_vector() != 0) {
        return 1;
    }

    if (test_vector_pop_empty_vector() != 0) {
        return 1;
    }

    if (test_vector_pop_null_out() != 0) {
        return 1;
    }

    if (test_vector_pop_returns_last_value() != 0) {
        return 1;
    }

    return 0;
}
