#include <stddef.h>
#include <stdio.h>

#include "datastruct/queue.h"

static int test_queue_pushfront_null_queue(void) {
    int value = 1;

    if (cstd_queue_pushfront(NULL, &value) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_pushfront(NULL, &value) should return CSTD_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_queue_pushfront_null_element(void) {
    enum cstd_status status;
    struct cstd_queue q;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_queue_pushfront(&q, NULL);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_pushfront(&q, NULL) should return CSTD_ERR_NULL\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

static int test_queue_pushfront_empty_queue(void) {
    enum cstd_status status;
    struct cstd_queue q;
    int value = 42;
    int out = 0;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_queue_pushfront(&q, &value);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_pushfront(&q, &value) should return CSTD_OK\n");
        cstd_queue_free(&q);
        return 1;
    }

    if (cstd_queue_size(&q) != 1) {
        fprintf(stderr, "cstd_queue_pushfront should increment queue size\n");
        cstd_queue_free(&q);
        return 1;
    }

    status = cstd_queue_popleft(&q, &out);
    if (status != CSTD_OK || out != value) {
        fprintf(stderr, "cstd_queue_pushfront should place value at queue front\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

static int test_queue_pushfront_preserves_front_order(void) {
    enum cstd_status status;
    struct cstd_queue q;
    int first = 1;
    int second = 2;
    int front = 0;
    int out = 0;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    if (cstd_queue_push(&q, &first) != CSTD_OK || cstd_queue_push(&q, &second) != CSTD_OK ||
        cstd_queue_pushfront(&q, &front) != CSTD_OK) {
        fprintf(stderr, "queue pushes should succeed before order checks\n");
        cstd_queue_free(&q);
        return 1;
    }

    status = cstd_queue_popleft(&q, &out);
    if (status != CSTD_OK || out != front) {
        fprintf(stderr, "cstd_queue_pushfront should return new front first\n");
        cstd_queue_free(&q);
        return 1;
    }

    status = cstd_queue_popleft(&q, &out);
    if (status != CSTD_OK || out != first) {
        fprintf(stderr, "cstd_queue should preserve existing order after pushfront\n");
        cstd_queue_free(&q);
        return 1;
    }

    status = cstd_queue_popleft(&q, &out);
    if (status != CSTD_OK || out != second) {
        fprintf(stderr, "cstd_queue should keep tail order after pushfront\n");
        cstd_queue_free(&q);
        return 1;
    }

    cstd_queue_free(&q);
    return 0;
}

static int test_queue_pushfront_wraparound_growth(void) {
    enum cstd_status status;
    struct cstd_queue q;
    size_t initial_capacity;
    int out = 0;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    initial_capacity = q.capacity;

    for (int i = 0; i < (int)initial_capacity; i++) {
        int value = i;
        if (cstd_queue_push(&q, &value) != CSTD_OK) {
            fprintf(stderr, "cstd_queue_push should fill queue capacity\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    for (int i = 0; i < (int)(initial_capacity / 2); i++) {
        if (cstd_queue_popleft(&q, &out) != CSTD_OK || out != i) {
            fprintf(stderr, "cstd_queue_popleft should consume initial prefix\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    for (int i = (int)initial_capacity; i < (int)(initial_capacity + (initial_capacity / 2)); i++) {
        int value = i;
        if (cstd_queue_push(&q, &value) != CSTD_OK) {
            fprintf(stderr, "cstd_queue_push should wrap and refill queue\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    {
        int front = -1;
        status = cstd_queue_pushfront(&q, &front);
        if (status != CSTD_OK) {
            fprintf(stderr, "cstd_queue_pushfront should grow wrapped full queue\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    if (q.capacity != initial_capacity * 2 || q.size != initial_capacity + 1) {
        fprintf(stderr, "cstd_queue_pushfront should grow capacity and preserve size\n");
        cstd_queue_free(&q);
        return 1;
    }

    if (cstd_queue_popleft(&q, &out) != CSTD_OK || out != -1) {
        fprintf(stderr, "cstd_queue_pushfront should keep inserted front after growth\n");
        cstd_queue_free(&q);
        return 1;
    }

    for (int i = (int)(initial_capacity / 2); i < (int)(initial_capacity + (initial_capacity / 2));
         i++) {
        if (cstd_queue_popleft(&q, &out) != CSTD_OK || out != i) {
            fprintf(stderr,
                    "cstd_queue_pushfront should preserve logical FIFO order after growth\n");
            cstd_queue_free(&q);
            return 1;
        }
    }

    cstd_queue_free(&q);
    return 0;
}

int main(void) {
    if (test_queue_pushfront_null_queue() != 0) {
        return 1;
    }

    if (test_queue_pushfront_null_element() != 0) {
        return 1;
    }

    if (test_queue_pushfront_empty_queue() != 0) {
        return 1;
    }

    if (test_queue_pushfront_preserves_front_order() != 0) {
        return 1;
    }

    if (test_queue_pushfront_wraparound_growth() != 0) {
        return 1;
    }

    return 0;
}
