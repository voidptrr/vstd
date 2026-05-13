#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "datastruct/queue.h"

static int test_queue_push_appends_value(void) {
    cstd_status status;
    cstd_queue q;
    int value = 42;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_queue_push(&q, &value);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_push(&q, &value) should return CSTD_OK\n");
        free(q.buffer);
        return 1;
    }

    if (q.size != 1) {
        fprintf(stderr, "queue_push should increment queue size\n");
        free(q.buffer);
        return 1;
    }

    if (q.head != 0 || q.tail != 1) {
        fprintf(stderr, "queue_push should update head/tail correctly\n");
        free(q.buffer);
        return 1;
    }

    if (((int *)q.buffer)[0] != value) {
        fprintf(stderr, "queue_push should copy pushed value into buffer\n");
        free(q.buffer);
        return 1;
    }

    free(q.buffer);
    return 0;
}

static int test_queue_push_null_pointers(void) {
    cstd_status status;
    cstd_queue q;
    int value = 1;

    status = cstd_queue_push(NULL, &value);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_push(NULL, &value) should return CSTD_ERR_NULL\n");
        return 1;
    }

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_queue_push(&q, NULL);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_push(&q, NULL) should return CSTD_ERR_NULL\n");
        free(q.buffer);
        return 1;
    }

    free(q.buffer);
    return 0;
}

static int test_queue_push_wraparound_growth(void) {
    cstd_status status;
    cstd_queue q;
    int pushed = 99;
    int popped = 0;
    int i;
    size_t initial_capacity;
    size_t pop_count;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    initial_capacity = q.capacity;
    pop_count = initial_capacity / 2;

    for (i = 0; i < (int)initial_capacity; i++) {
        int value = i;
        status = cstd_queue_push(&q, &value);
        if (status != CSTD_OK) {
            fprintf(stderr, "cstd_queue_push should fill queue up to capacity\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    for (i = 0; i < (int)pop_count; i++) {
        status = cstd_queue_popleft(&q, &popped);
        if (status != CSTD_OK || popped != i) {
            fprintf(stderr, "cstd_queue_popleft should dequeue initial FIFO values\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    for (i = (int)initial_capacity; i < (int)(initial_capacity + pop_count); i++) {
        int value = i;
        status = cstd_queue_push(&q, &value);
        if (status != CSTD_OK) {
            fprintf(stderr, "cstd_queue_push should wrap and refill queue\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    status = cstd_queue_push(&q, &pushed);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_push should succeed on wrapped full queue\n");
        cstd_queue_free(&q);
        return 1;
    }

    if (q.capacity != initial_capacity * 2 || q.size != initial_capacity + 1 || q.head != 0 ||
        q.tail != initial_capacity + 1) {
        fprintf(stderr, "cstd_queue_push should grow and normalize wrapped queue indices\n");
        cstd_queue_free(&q);
        return 1;
    }

    for (i = (int)pop_count; i < (int)(initial_capacity + pop_count); i++) {
        status = cstd_queue_popleft(&q, &popped);
        if (status != CSTD_OK || popped != i) {
            fprintf(stderr, "cstd_queue should preserve FIFO order after wrapped growth\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    status = cstd_queue_popleft(&q, &popped);
    if (status != CSTD_OK || popped != pushed) {
        fprintf(stderr, "cstd_queue should keep newly pushed element after growth\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

int main(void) {
    if (test_queue_push_appends_value() != 0) {
        return 1;
    }

    if (test_queue_push_null_pointers() != 0) {
        return 1;
    }

    if (test_queue_push_wraparound_growth() != 0) {
        return 1;
    }

    return 0;
}
