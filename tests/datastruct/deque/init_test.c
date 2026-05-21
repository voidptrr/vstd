#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque q;

    ckit_deque_init(&q, sizeof(int), NULL);
    if (ckit_deque_size(&q) != 0U || !ckit_deque_is_empty(&q)) {
        fprintf(stderr, "deque should initialize empty\n");
        ckit_deque_free(&q);
        return 1;
    }

    ckit_deque_free(&q);
    return 0;
}
