#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "datastruct/vector.h"

#define CSTD_VECTOR_DEFAULT_CAPACITY 16

cstd_status cstd_vector_init(cstd_vector *vector, size_t elem_size) {
    if (vector == NULL) {
        return CSTD_ERR_NULL;
    }

    void *buffer = malloc(elem_size * CSTD_VECTOR_DEFAULT_CAPACITY);
    if (buffer == NULL) {
        return CSTD_ERR_OOM;
    }

    vector->buffer = buffer;
    vector->size = 0;
    vector->elem_size = elem_size;
    vector->capacity = CSTD_VECTOR_DEFAULT_CAPACITY;

    return CSTD_OK;
}

cstd_status cstd_vector_push(cstd_vector *vector, const void *element) {
    if (vector == NULL || element == NULL) {
        return CSTD_ERR_NULL;
    }

    if (vector->size == vector->capacity) {
        size_t new_capacity = vector->capacity * 2;
        void *tmp = realloc(vector->buffer, new_capacity * vector->elem_size);
        if (tmp == NULL) {
            return CSTD_ERR_OOM;
        }

        vector->buffer = tmp;
        vector->capacity = new_capacity;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *dst = base + (vector->size * vector->elem_size);
    memcpy(dst, element, vector->elem_size);
    vector->size += 1;

    return CSTD_OK;
}

cstd_status cstd_vector_pop(cstd_vector *vector, void *out) {
    if (vector == NULL || out == NULL) {
        return CSTD_ERR_NULL;
    }

    if (vector->size == 0) {
        return CSTD_ERR_EMPTY;
    }

    vector->size -= 1;

    uint8_t *base = (uint8_t *)vector->buffer;
    void *src = base + (vector->size * vector->elem_size);
    memcpy(out, src, vector->elem_size);

    return CSTD_OK;
}

cstd_status cstd_vector_free(cstd_vector *vector) {
    if (vector == NULL) {
        return CSTD_ERR_NULL;
    }

    free(vector->buffer);
    vector->buffer = NULL;
    vector->size = 0;
    vector->capacity = 0;

    return CSTD_OK;
}

size_t cstd_vector_size(const cstd_vector *vector) {
    if (vector == NULL) {
        return 0;
    }

    return vector->size;
}
