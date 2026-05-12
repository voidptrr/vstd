#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

enum vector_status { VECTOR_OK, VECTOR_ERR_OOM, VECTOR_ERR_NULL, VECTOR_ERR_EMPTY };

/*
 * Generic contiguous dynamic array storage.
 * +----------------------+----------------------+----------------------+----------------------+
 * | size (size_t)        | elem_size (size_t)   | capacity (size_t)    | buffer (void *)      |
 * +----------------------+----------------------+----------------------+----------------------+
 */
struct vector {
    size_t size;
    size_t elem_size;
    size_t capacity;
    void *buffer;
};

enum vector_status vector_init(struct vector *vector, size_t elem_size);
enum vector_status vector_push(struct vector *vector, const void *element);
enum vector_status vector_pop(struct vector *vector, void *out);
enum vector_status vector_free(struct vector *vector);
size_t vector_size(struct vector *vector);

#endif
