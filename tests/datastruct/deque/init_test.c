#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    int status = 0;
    ck_deque *q;

    q = ck_deque_init(sizeof(int), NULL);
    if (ck_deque_size(q) != 0) {
        fprintf(stderr, "deque should initialize empty\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_deque_deinit(q);
    return status;
}
