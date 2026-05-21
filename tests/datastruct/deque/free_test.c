#include <stdio.h>

#include "ckit/datastruct/deque.h"

int main(void) {
    ckit_deque *q;
    int value = 1;

    q = ckit_deque_init(sizeof(int), NULL);
    ckit_deque_push(q, &value);
    ckit_deque_free(q);

    return 0;
}
