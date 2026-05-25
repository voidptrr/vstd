#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    int status = 0;
    ck_deque *q;
    int value = 42;

    q = ck_deque_init(sizeof(int), NULL);
    ck_deque_push(q, &value);

    const int *out = ck_deque_peekback(q);
    if (ck_deque_size(q) != 1 || out == NULL || *out != value) {
        fprintf(stderr, "push should append element\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_deque_deinit(q);
    return status;
}
