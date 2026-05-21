#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    ckit_vector v;
    size_t initial_capacity;

    ckit_vector_init(&v, sizeof(int), NULL);
    initial_capacity = v.capacity;

    for (size_t i = 0; i < initial_capacity + 1U; i++) {
        int value = (int)i;
        ckit_vector_push(&v, &value);
    }

    if (v.capacity <= initial_capacity || v.size != initial_capacity + 1U) {
        fprintf(stderr, "vector should grow when pushing past capacity\n");
        ckit_vector_free(&v);
        return 1;
    }

    ckit_vector_free(&v);
    return 0;
}
