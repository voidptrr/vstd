#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    ckit_vector *v;
    int value = 1;

    v = ckit_vector_init(sizeof(int), NULL);
    if (ckit_vector_size(v) != 0) {
        fprintf(stderr, "vector should initialize empty\n");
        ckit_vector_free(v);
        return 1;
    }

    ckit_vector_push(v, &value);
    if (ckit_vector_size(v) != 1) {
        fprintf(stderr, "vector should contain one element after push\n");
        ckit_vector_free(v);
        return 1;
    }

    ckit_vector_free(v);
    return 0;
}
