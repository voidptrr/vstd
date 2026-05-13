#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "datastruct/queue.h"

static int test_queue_init_null_pointer(void) {
    cstd_status status;

    status = cstd_queue_init(NULL, sizeof(int));
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_queue_init(NULL, ...) should return CSTD_ERR_NULL\n");
        return 1;
    }

    return 0;
}

static int test_queue_init_valid_pointer(void) {
    cstd_status status;
    cstd_queue q;

    status = cstd_queue_init(&q, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_queue_init(&q, ...) should return CSTD_OK\n");
        return 1;
    }

    if (q.buffer == NULL || q.size != 0 || q.elem_size != sizeof(int) || q.capacity != 16 ||
        q.head != 0 || q.tail != 0) {
        fprintf(stderr, "cstd_queue_init(&q, ...) returned invalid queue state\n");
        free(q.buffer);
        return 1;
    }

    free(q.buffer);
    return 0;
}

int main(void) {
    if (test_queue_init_null_pointer() != 0) {
        return 1;
    }

    if (test_queue_init_valid_pointer() != 0) {
        return 1;
    }

    return 0;
}
