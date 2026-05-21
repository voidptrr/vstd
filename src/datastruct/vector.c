#include <stdint.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/vector.h"
#include "ckit/memory/allocators/allocator.h"

#define CKIT_VECTOR_DEFAULT_CAPACITY 16

struct ckit_vector {
    size_t size;
    size_t elem_size;
    size_t capacity;
    void *buffer;
    ckit_allocator *allocator;
};

ckit_vector *ckit_vector_init(size_t elem_size, ckit_allocator *allocator) {
    CKIT_ASSERT(elem_size > 0U, "fatal: ckit_vector_init invalid arguments");

    ckit_vector *vector = ckit_malloc(allocator, sizeof(*vector));
    vector->allocator = allocator;

    size_t alloc_size = elem_size * CKIT_VECTOR_DEFAULT_CAPACITY;
    void *buffer = ckit_malloc(allocator, alloc_size);

    vector->buffer = buffer;
    vector->size = 0;
    vector->elem_size = elem_size;
    vector->capacity = CKIT_VECTOR_DEFAULT_CAPACITY;

    return vector;
}

void ckit_vector_push(ckit_vector *vector, const void *element) {
    CKIT_ASSERT(vector != NULL, "fatal: ckit_vector_push invalid arguments");
    CKIT_ASSERT(element != NULL, "fatal: ckit_vector_push invalid arguments");

    if (vector->size == vector->capacity) {
        size_t new_capacity = vector->capacity * 2;
        size_t alloc_size = new_capacity * vector->elem_size;

        void *tmp = ckit_realloc(vector->allocator, vector->buffer, alloc_size);
        vector->buffer = tmp;
        vector->capacity = new_capacity;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *dst = base + (vector->size * vector->elem_size);
    memcpy(dst, element, vector->elem_size);
    vector->size += 1;
}

void *ckit_vector_pop(ckit_vector *vector) {
    CKIT_ASSERT(vector != NULL, "fatal: ckit_vector_pop invalid arguments");

    if (vector->size == 0) {
        return NULL;
    }

    vector->size -= 1;

    uint8_t *base = (uint8_t *)vector->buffer;
    return base + (vector->size * vector->elem_size);
}

void *ckit_vector_get(ckit_vector *vector, size_t index) {
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    return base + (index * vector->elem_size);
}

const void *ckit_vector_get_const(const ckit_vector *vector, size_t index) {
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    const uint8_t *base = (const uint8_t *)vector->buffer;
    return base + (index * vector->elem_size);
}

size_t ckit_vector_elem_size(const ckit_vector *vector) {
    if (vector == NULL) {
        return 0U;
    }

    return vector->elem_size;
}

void *ckit_vector_swap_remove(ckit_vector *vector, size_t index) {
    CKIT_ASSERT(vector != NULL, "fatal: ckit_vector_swap_remove invalid arguments");

    if (index >= vector->size) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *slot = base + (index * vector->elem_size);
    size_t last = vector->size - 1U;

    if (index != last) {
        void *last_slot = base + (last * vector->elem_size);
        memcpy(slot, last_slot, vector->elem_size);
    }

    vector->size = last;
    return slot;
}

void ckit_vector_free(ckit_vector *vector) {
    CKIT_ASSERT(vector != NULL, "fatal: ckit_vector_free invalid arguments");

    ckit_allocator *allocator = vector->allocator;
    ckit_dealloc(vector->allocator, vector->buffer);
    ckit_dealloc(allocator, vector);
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
