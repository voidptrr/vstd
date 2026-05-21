#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque q;
    int value = 1;

    ckit_deque_init(&q, sizeof(int), NULL);
    ckit_deque_push(&q, &value);
    ckit_deque_free(&q);

    if (q.buffer != NULL || q.size != 0U || q.capacity != 0U) {
        fprintf(stderr, "free should reset deque state\n");
        return 1;
    }

    return 0;
}
