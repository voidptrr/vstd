#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    ckit_vector *v;

    v = ckit_vector_init(sizeof(int), NULL);

    for (size_t i = 0; i < 17U; i++) {
        int value = (int)i;
        ckit_vector_push(v, &value);
    }

    if (ckit_vector_size(v) != 17U) {
        fprintf(stderr, "vector should store all pushed elements\n");
        ckit_vector_free(v);
        return 1;
    }

    ckit_vector_free(v);
    return 0;
}
