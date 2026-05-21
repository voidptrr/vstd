#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque *q;
    int first = 1;
    int second = 2;
    int *out;

    q = ckit_deque_init(sizeof(int), NULL);
    ckit_deque_push(q, &first);
    ckit_deque_push(q, &second);

    out = (int *)ckit_deque_popback(q);
    if (out == NULL || *out != second) {
        fprintf(stderr, "popback should remove back element\n");
        ckit_deque_free(q);
        return 1;
    }

    ckit_deque_free(q);
    return 0;
}
