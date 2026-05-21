#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    ckit_vector *v;
    v = ckit_vector_init(sizeof(int), NULL);
    ckit_vector_free(v);

    return 0;
}
