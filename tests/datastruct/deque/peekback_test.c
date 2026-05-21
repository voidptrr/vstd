#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque q;
    int first = 1;
    int second = 2;
    const int *out;

    ckit_deque_init(&q, sizeof(int), NULL);
    ckit_deque_push(&q, &first);
    ckit_deque_push(&q, &second);

    out = (const int *)ckit_deque_peekback(&q);
    if (out == NULL || *out != second) {
        fprintf(stderr, "peekback should return back\n");
        ckit_deque_free(&q);
        return 1;
    }

    ckit_deque_free(&q);
    return 0;
}
