#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    ckit_vector v;
    int first = 7;
    int second = 11;
    int *popped;

    ckit_vector_init(&v, sizeof(int), NULL);
    if (ckit_vector_pop(&v) != NULL) {
        fprintf(stderr, "empty vector pop should return NULL\n");
        ckit_vector_free(&v);
        return 1;
    }

    ckit_vector_push(&v, &first);
    ckit_vector_push(&v, &second);

    popped = (int *)ckit_vector_pop(&v);
    if (popped == NULL || *popped != second) {
        fprintf(stderr, "vector pop should return last value\n");
        ckit_vector_free(&v);
        return 1;
    }

    popped = (int *)ckit_vector_pop(&v);
    if (popped == NULL || *popped != first) {
        fprintf(stderr, "vector pop should preserve LIFO order\n");
        ckit_vector_free(&v);
        return 1;
    }

    ckit_vector_free(&v);
    return 0;
}
