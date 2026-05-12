#include <stddef.h>
#include <stdio.h>

#include "queue.h"

static int test_queue_popback_null_queue(void) {
    int out = 0;

    if (queue_popback(NULL, &out) != QUEUE_ERR_NULL) {
        fprintf(stderr, "queue_popback(NULL, &out) should return QUEUE_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_queue_popback_null_out(void) {
    enum queue_status status;
    struct queue q;

    status = queue_init(&q, sizeof(int));
    if (status != QUEUE_OK) {
        fprintf(stderr, "queue_init(&q, ...) should return QUEUE_OK\n");
        return 1;
    }

    status = queue_popback(&q, NULL);
    if (status != QUEUE_ERR_NULL) {
        fprintf(stderr, "queue_popback(&q, NULL) should return QUEUE_ERR_NULL\n");
        queue_free(&q);
        return 1;
    }

    queue_free(&q);
    return 0;
}

static int test_queue_popback_empty_queue(void) {
    enum queue_status status;
    struct queue q;
    int out = 0;

    status = queue_init(&q, sizeof(int));
    if (status != QUEUE_OK) {
        fprintf(stderr, "queue_init(&q, ...) should return QUEUE_OK\n");
        return 1;
    }

    status = queue_popback(&q, &out);
    if (status != QUEUE_ERR_EMPTY) {
        fprintf(stderr, "queue_popback on empty queue should return QUEUE_ERR_EMPTY\n");
        queue_free(&q);
        return 1;
    }

    queue_free(&q);
    return 0;
}

static int test_queue_popback_wraps_tail_backward(void) {
    enum queue_status status;
    struct queue q;
    int out = 0;
    int i;

    status = queue_init(&q, sizeof(int));
    if (status != QUEUE_OK) {
        fprintf(stderr, "queue_init(&q, ...) should return QUEUE_OK\n");
        return 1;
    }

    for (i = 0; i < 50; i++) {
        int value = i;
        status = queue_push(&q, &value);
        if (status != QUEUE_OK) {
            fprintf(stderr, "queue_push should fill queue capacity\n");
            queue_free(&q);
            return 1;
        }
    }

    status = queue_popback(&q, &out);
    if (status != QUEUE_OK || out != 49) {
        fprintf(stderr, "queue_popback should return the last pushed element\n");
        queue_free(&q);
        return 1;
    }

    if (q.tail != 49 || q.size != 49) {
        fprintf(stderr, "queue_popback should move tail backward by one\n");
        queue_free(&q);
        return 1;
    }

    for (i = 0; i < 49; i++) {
        status = queue_popleft(&q, &out);
        if (status != QUEUE_OK || out != i) {
            fprintf(stderr, "queue should preserve remaining FIFO order after popback\n");
            queue_free(&q);
            return 1;
        }
    }

    queue_free(&q);
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
