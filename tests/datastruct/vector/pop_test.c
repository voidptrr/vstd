#include <stddef.h>
#include <stdio.h>

#include "ckit/datastruct/vector.h"

static int test_vector_pop_null_vector(void) {
    int out = 0;

    if (ckit_vector_pop(NULL, &out) != CKIT_ERR_NULL) {
        fprintf(stderr, "ckit_vector_pop(NULL, &out) should return CKIT_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_vector_pop_null_out(void) {
    ckit_status status;
    ckit_vector v;

    status = ckit_vector_init(&v, sizeof(int), NULL);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_init(&v, ..., NULL) should return CKIT_OK\n");
        return 1;
    }

    status = ckit_vector_pop(&v, NULL);
    if (status != CKIT_ERR_NULL) {
        fprintf(stderr, "ckit_vector_pop(&v, NULL) should return CKIT_ERR_NULL\n");
        ckit_vector_free(&v);
        return 1;
    }

    if (ckit_vector_free(&v) != CKIT_OK) {
        fprintf(stderr, "ckit_vector_free(&v) should return CKIT_OK\n");
        return 1;
    }

    return 0;
}

static int test_vector_pop_empty_vector(void) {
    ckit_status status;
    ckit_vector v;

    status = ckit_vector_init(&v, sizeof(int), NULL);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_init(&v, ..., NULL) should return CKIT_OK\n");
        return 1;
    }

    {
        int out = 0;
        if (ckit_vector_pop(&v, &out) != CKIT_ERR_EMPTY) {
            fprintf(stderr, "ckit_vector_pop on empty vector should return CKIT_ERR_EMPTY\n");
            ckit_vector_free(&v);
            return 1;
        }
    }

    if (ckit_vector_size(&v) != 0) {
        fprintf(stderr, "ckit_vector_pop on empty vector should not change size\n");
        ckit_vector_free(&v);
        return 1;
    }

    if (ckit_vector_free(&v) != CKIT_OK) {
        fprintf(stderr, "ckit_vector_free(&v) should return CKIT_OK\n");
        return 1;
    }

    return 0;
}

static int test_vector_pop_returns_last_value(void) {
    ckit_status status;
    ckit_vector v;
    int first = 7;
    int second = 11;
    int popped = 0;

    status = ckit_vector_init(&v, sizeof(int), NULL);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_init(&v, ..., NULL) should return CKIT_OK\n");
        return 1;
    }

    status = ckit_vector_push(&v, &first);
    if (status != CKIT_OK) {
        fprintf(stderr, "first ckit_vector_push should return CKIT_OK\n");
        ckit_vector_free(&v);
        return 1;
    }

    status = ckit_vector_push(&v, &second);
    if (status != CKIT_OK) {
        fprintf(stderr, "second ckit_vector_push should return CKIT_OK\n");
        ckit_vector_free(&v);
        return 1;
    }

    status = ckit_vector_pop(&v, &popped);
    if (status != CKIT_OK) {
        fprintf(stderr, "ckit_vector_pop should return CKIT_OK\n");
        ckit_vector_free(&v);
        return 1;
    }

    if (popped != second) {
        fprintf(stderr, "ckit_vector_pop should return last pushed value\n");
        ckit_vector_free(&v);
        return 1;
    }

    if (ckit_vector_size(&v) != 1) {
        fprintf(stderr, "ckit_vector_pop should decrement vector size\n");
        ckit_vector_free(&v);
        return 1;
    }

    status = ckit_vector_pop(&v, &popped);
    if (status != CKIT_OK || popped != first) {
        fprintf(stderr, "ckit_vector_pop should preserve LIFO order\n");
        ckit_vector_free(&v);
        return 1;
    }

    if (ckit_vector_size(&v) != 0) {
        fprintf(stderr, "ckit_vector should be empty after popping all elements\n");
        ckit_vector_free(&v);
        return 1;
    }

    if (ckit_vector_free(&v) != CKIT_OK) {
        fprintf(stderr, "ckit_vector_free(&v) should return CKIT_OK\n");
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
