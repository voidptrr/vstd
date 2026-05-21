#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque q;
    int first = 1;
    int second = 2;
    int *out;

    ckit_deque_init(&q, sizeof(int), NULL);
    ckit_deque_push(&q, &first);
    ckit_deque_pushfront(&q, &second);

    out = (int *)ckit_deque_popleft(&q);
    if (out == NULL || *out != second) {
        fprintf(stderr, "pushfront should prepend\n");
        ckit_deque_free(&q);
        return 1;
    }

    ckit_deque_free(&q);
    return 0;
}
