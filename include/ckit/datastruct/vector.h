#ifndef CKIT_DATASTRUCT_VECTOR_H
#define CKIT_DATASTRUCT_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/mem/allocators/allocator.h"

/*
 * Generic contiguous dynamic array storage.
 * +----------------------+----------------------+----------------------+----------------------+
 * | size (size_t)        | elem_size (size_t)   | capacity (size_t)    | buffer (void *)      |
 * +----------------------+----------------------+----------------------+----------------------+
 */
typedef struct ckit_vector {
    size_t size;
    size_t elem_size;
    size_t capacity;
    void *buffer;
    ckit_allocator *allocator;
} ckit_vector;

/* Initialize a vector with element size elem_size. */
void ckit_vector_init(ckit_vector *vector, size_t elem_size, ckit_allocator *allocator);

/* Append one element by copying elem_size bytes from element. */
void ckit_vector_push(ckit_vector *vector, const void *element);

/* Remove and return the last element pointer, or NULL when empty. */
void *ckit_vector_pop(ckit_vector *vector);

/* Release owned storage and reset vector to an empty state. */
void ckit_vector_free(ckit_vector *vector);

/* Return the number of stored elements. */
size_t ckit_vector_size(const ckit_vector *vector);

/* Return whether the vector has zero elements. */
bool ckit_vector_is_empty(const ckit_vector *vector);

#endif
