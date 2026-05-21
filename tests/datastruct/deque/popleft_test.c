#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque q;
    int values[] = {10, 20};
    int *out;

    ckit_deque_init(&q, sizeof(int), NULL);
    if (ckit_deque_popleft(&q) != NULL) {
        fprintf(stderr, "popleft on empty deque should return NULL\n");
        ckit_deque_free(&q);
        return 1;
    }

    ckit_deque_push(&q, &values[0]);
    ckit_deque_push(&q, &values[1]);
    out = (int *)ckit_deque_popleft(&q);
    if (out == NULL || *out != values[0]) {
        fprintf(stderr, "popleft should dequeue FIFO\n");
        ckit_deque_free(&q);
        return 1;
    }

    ckit_deque_free(&q);
    return 0;
}
