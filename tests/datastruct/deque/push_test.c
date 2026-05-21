#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque q;
    int value = 42;

    ckit_deque_init(&q, sizeof(int), NULL);
    ckit_deque_push(&q, &value);

    if (ckit_deque_size(&q) != 1U || *(int *)q.buffer != value) {
        fprintf(stderr, "push should append element\n");
        ckit_deque_free(&q);
        return 1;
    }

    ckit_deque_free(&q);
    return 0;
}
