#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    ckit_vector v;
    int value = 1;

    if (!ckit_vector_is_empty(NULL)) {
        fprintf(stderr, "ckit_vector_is_empty(NULL) should be true\n");
        return 1;
    }

    ckit_vector_init(&v, sizeof(int), NULL);
    if (ckit_vector_size(&v) != 0U || !ckit_vector_is_empty(&v)) {
        fprintf(stderr, "vector should initialize empty\n");
        ckit_vector_free(&v);
        return 1;
    }

    ckit_vector_push(&v, &value);
    if (ckit_vector_is_empty(&v)) {
        fprintf(stderr, "vector should not be empty after push\n");
        ckit_vector_free(&v);
        return 1;
    }

    ckit_vector_free(&v);
    return 0;
}
