#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    ckit_vector v;
    ckit_vector_init(&v, sizeof(int), NULL);
    ckit_vector_free(&v);

    if (v.buffer != NULL || v.size != 0U || v.capacity != 0U) {
        fprintf(stderr, "vector free should reset state\n");
        return 1;
    }

    return 0;
}
