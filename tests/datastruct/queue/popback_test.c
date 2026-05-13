#include <stddef.h>
#include <stdio.h>

#include "datastruct/queue.h"

static int test_queue_popback_null_queue(void) {
    int out = 0;

    if (cstd_queue_popback(NULL, &out) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_popback(NULL, &out) should return CSTD_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_queue_popback_null_out(void) {
    cstd_status status;
    cstd_queue q;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_queue_popback(&q, NULL);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_popback(&q, NULL) should return CSTD_ERR_NULL\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

static int test_queue_popback_empty_queue(void) {
    cstd_status status;
    cstd_queue q;
    int out = 0;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_queue_popback(&q, &out);
    if (status != CSTD_ERR_EMPTY) {
        fprintf(stderr, "cstd_queue_popback on empty queue should return CSTD_ERR_EMPTY\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

static int test_queue_popback_wraps_tail_backward(void) {
    cstd_status status;
    cstd_queue q;
    int out = 0;
    int i;
    size_t initial_capacity;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    initial_capacity = q.capacity;

    for (i = 0; i < (int)initial_capacity; i++) {
        int value = i;
        status = cstd_queue_push(&q, &value);
        if (status != CSTD_OK) {
            fprintf(stderr, "cstd_queue_push should fill queue capacity\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    status = cstd_queue_popback(&q, &out);
    if (status != CSTD_OK || out != (int)(initial_capacity - 1)) {
        fprintf(stderr, "cstd_queue_popback should return the last pushed element\n");
        cstd_queue_free(&q);
        return 1;
    }

    if (q.tail != initial_capacity - 1 || q.size != initial_capacity - 1) {
        fprintf(stderr, "cstd_queue_popback should move tail backward by one\n");
        cstd_queue_free(&q);
        return 1;
    }

    for (i = 0; i < (int)(initial_capacity - 1); i++) {
        status = cstd_queue_popleft(&q, &out);
        if (status != CSTD_OK || out != i) {
            fprintf(stderr, "cstd_queue should preserve remaining FIFO order after popback\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    cstd_queue_free(&q);
    return 0;
}

int main(void) {
    if (test_queue_popback_null_queue() != 0) {
        return 1;
    }

    if (test_queue_popback_null_out() != 0) {
        return 1;
    }

    if (test_queue_popback_empty_queue() != 0) {
        return 1;
    }

    if (test_queue_popback_wraps_tail_backward() != 0) {
        return 1;
    }

    return 0;
}
