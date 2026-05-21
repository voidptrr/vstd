#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque *q;
    int value = 42;

    q = ckit_deque_init(sizeof(int), NULL);
    ckit_deque_push(q, &value);

    const int *out = ckit_deque_peekback(q);
    if (ckit_deque_size(q) != 1U || out == NULL || *out != value) {
        fprintf(stderr, "push should append element\n");
        ckit_deque_free(q);
        return 1;
    }

    ckit_deque_free(q);
    return 0;
}
