#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ckit/datastruct/vector.h"
#include "ckit/mem/allocators/allocator.h"

#define CKIT_VECTOR_DEFAULT_CAPACITY 16

static void *ckit_vector_alloc(const ckit_vector *vector, size_t size) {
    if (vector->allocator == NULL || vector->allocator->alloc == NULL) {
        return ckit_malloc(size);
    }
    return vector->allocator->alloc(vector->allocator->ctx, size);
}

static void *ckit_vector_realloc(const ckit_vector *vector, void *ptr, size_t size) {
    if (vector->allocator == NULL || vector->allocator->realloc == NULL) {
        return ckit_realloc(ptr, size);
    }
    return vector->allocator->realloc(vector->allocator->ctx, ptr, size);
}

static void ckit_vector_dealloc(const ckit_vector *vector, void *ptr) {
    if (vector->allocator == NULL || vector->allocator->dealloc == NULL) {
        free(ptr);
        return;
    }
    vector->allocator->dealloc(vector->allocator->ctx, ptr);
}

ckit_status ckit_vector_init(ckit_vector *vector, size_t elem_size, ckit_allocator *allocator) {
    if (vector == NULL) {
        return CKIT_ERR_NULL;
    }

    if (elem_size == 0) {
        return CKIT_ERR_RANGE;
    }

    vector->allocator = allocator;

    void *buffer = ckit_vector_alloc(vector, elem_size * CKIT_VECTOR_DEFAULT_CAPACITY);
    if (buffer == NULL) {
        return CKIT_ERR_RANGE;
    }

    vector->buffer = buffer;
    vector->size = 0;
    vector->elem_size = elem_size;
    vector->capacity = CKIT_VECTOR_DEFAULT_CAPACITY;

    return CKIT_OK;
}

ckit_status ckit_vector_push(ckit_vector *vector, const void *element) {
    if (vector == NULL || element == NULL) {
        return CKIT_ERR_NULL;
    }

    if (vector->size == vector->capacity) {
        size_t new_capacity = vector->capacity * 2;
        void *tmp = ckit_vector_realloc(vector, vector->buffer, new_capacity * vector->elem_size);
        if (tmp == NULL) {
            return CKIT_ERR_RANGE;
        }

        vector->buffer = tmp;
        vector->capacity = new_capacity;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *dst = base + (vector->size * vector->elem_size);
    memcpy(dst, element, vector->elem_size);
    vector->size += 1;

    return CKIT_OK;
}

ckit_status ckit_vector_pop(ckit_vector *vector, void *out) {
    if (vector == NULL || out == NULL) {
        return CKIT_ERR_NULL;
    }

    if (vector->size == 0) {
        return CKIT_ERR_EMPTY;
    }

    vector->size -= 1;

    uint8_t *base = (uint8_t *)vector->buffer;
    void *src = base + (vector->size * vector->elem_size);
    memcpy(out, src, vector->elem_size);

    return CKIT_OK;
}

ckit_status ckit_vector_free(ckit_vector *vector) {
    if (vector == NULL) {
        return CKIT_ERR_NULL;
    }

    ckit_vector_dealloc(vector, vector->buffer);
    vector->buffer = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->allocator = NULL;

    return CKIT_OK;
}

size_t ckit_vector_size(const ckit_vector *vector) {
    if (vector == NULL) {
        return 0;
    }

    return vector->size;
}

bool ckit_vector_is_empty(const ckit_vector *vector) {
    return ckit_vector_size(vector) == 0;
}
