#ifndef CSTD_DATASTRUCT_QUEUE_H
#define CSTD_DATASTRUCT_QUEUE_H

#include <stddef.h>

#include "status.h"

/*
 * Generic FIFO queue backed by a circular buffer.
 * +----------------------+----------------------+----------------------+
 * | size (size_t)        | elem_size (size_t)   | capacity (size_t)    |
 * +----------------------+----------------------+----------------------+
 * | head (size_t)        | tail (size_t)        | buffer (void *)      |
 * +----------------------+----------------------+----------------------+
 */
typedef struct {
    size_t size;
    size_t elem_size;
    size_t capacity;
    size_t head;
    size_t tail;
    void *buffer;
} cstd_queue;

/* Initialize a queue with element size elem_size. */
cstd_status cstd_queue_init(cstd_queue *queue, size_t elem_size);

/* Enqueue one element by copying elem_size bytes from element. */
cstd_status cstd_queue_push(cstd_queue *queue, const void *element);

/* Dequeue one element from the front into out. */
cstd_status cstd_queue_popleft(cstd_queue *queue, void *out);

/* Remove one element from the back into out. */
cstd_status cstd_queue_popback(cstd_queue *queue, void *out);

/* Release owned storage and reset queue to an empty state. */
cstd_status cstd_queue_free(cstd_queue *queue);

/* Return the number of stored elements. */
size_t cstd_queue_size(const cstd_queue *queue);

#endif
