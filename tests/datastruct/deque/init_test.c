#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque *q;

    q = ckit_deque_init(sizeof(int), NULL);
    if (ckit_deque_size(q) != 0) {
        fprintf(stderr, "deque should initialize empty\n");
        ckit_deque_free(q);
        return 1;
    }

    ckit_deque_free(q);
    return 0;
}
