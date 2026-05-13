#include <stddef.h>
#include <stdio.h>

#include "datastruct/queue.h"

static int test_queue_popleft_null_queue(void) {
    int out = 0;

    if (cstd_queue_popleft(NULL, &out) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_popleft(NULL, &out) should return CSTD_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_queue_popleft_null_out(void) {
    cstd_status status;
    cstd_queue q;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_queue_popleft(&q, NULL);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_popleft(&q, NULL) should return CSTD_ERR_NULL\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

static int test_queue_popleft_empty_queue(void) {
    cstd_status status;
    cstd_queue q;
    int out = 0;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_queue_popleft(&q, &out);
    if (status != CSTD_ERR_EMPTY) {
        fprintf(stderr, "cstd_queue_popleft on empty queue should return CSTD_ERR_EMPTY\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

static int test_queue_popleft_fifo_order(void) {
    cstd_status status;
    cstd_queue q;
    int values[] = {7, 11, 13};
    int out = 0;
    size_t i;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    for (i = 0; i < 3; i++) {
        status = cstd_queue_push(&q, &values[i]);
        if (status != CSTD_OK) {
            fprintf(stderr, "cstd_queue_push should succeed before cstd_queue_popleft checks\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    for (i = 0; i < 3; i++) {
        status = cstd_queue_popleft(&q, &out);
        if (status != CSTD_OK || out != values[i]) {
            fprintf(stderr, "cstd_queue_popleft should preserve FIFO order\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    if (cstd_queue_size(&q) != 0) {
        fprintf(stderr, "cstd_queue should be empty after popping all elements\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

int main(void) {
    if (test_queue_popleft_null_queue() != 0) {
        return 1;
    }

    if (test_queue_popleft_null_out() != 0) {
        return 1;
    }

    if (test_queue_popleft_empty_queue() != 0) {
        return 1;
    }

    if (test_queue_popleft_fifo_order() != 0) {
        return 1;
    }

    return 0;
}
