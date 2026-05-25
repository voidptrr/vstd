#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    int status = 0;
    ck_deque *q;
    int values[] = {10, 20};
    int *out;

    q = ck_deque_init(sizeof(int), NULL);
    if (ck_deque_popleft(q) != NULL) {
        fprintf(stderr, "popleft on empty deque should return NULL\n");
        status = 1;
        goto cleanup;
    }

    ck_deque_push(q, &values[0]);
    ck_deque_push(q, &values[1]);
    out = (int *)ck_deque_popleft(q);
    if (out == NULL || *out != values[0]) {
        fprintf(stderr, "popleft should dequeue FIFO\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_deque_deinit(q);
    return status;
}
