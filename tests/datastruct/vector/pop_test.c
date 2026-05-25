#include <stdio.h>

#include "ckit/datastruct/vector.h"

int main(void) {
    int status = 0;
    ck_vector *v;
    int first = 7;
    int second = 11;
    int *popped;

    v = ck_vector_init(sizeof(int), NULL);
    if (ck_vector_pop(v) != NULL) {
        fprintf(stderr, "empty vector pop should return NULL\n");
        status = 1;
        goto cleanup;
    }

    ck_vector_push(v, &first);
    ck_vector_push(v, &second);

    popped = (int *)ck_vector_pop(v);
    if (popped == NULL || *popped != second) {
        fprintf(stderr, "vector pop should return last value\n");
        status = 1;
        goto cleanup;
    }

    popped = (int *)ck_vector_pop(v);
    if (popped == NULL || *popped != first) {
        fprintf(stderr, "vector pop should preserve LIFO order\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_vector_deinit(v);
    return status;
}
