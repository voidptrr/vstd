#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    int status = 0;
    ck_deque *q;
    int first = 1;
    int second = 2;
    const int *out;

    q = ck_deque_init(sizeof(int), NULL);
    ck_deque_push(q, &first);
    ck_deque_push(q, &second);

    out = (const int *)ck_deque_peekback(q);
    if (out == NULL || *out != second) {
        fprintf(stderr, "peekback should return back\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_deque_deinit(q);
    return status;
}
