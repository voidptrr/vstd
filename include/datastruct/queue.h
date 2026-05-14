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
struct cstd_queue {
    size_t size;
    size_t elem_size;
    size_t capacity;
    size_t head;
    size_t tail;
    void *buffer;
};

/* Initialize a queue with element size elem_size. */
enum cstd_status cstd_queue_init(struct cstd_queue *queue, size_t elem_size);

/* Enqueue one element by copying elem_size bytes from element. */
enum cstd_status cstd_queue_push(struct cstd_queue *queue, const void *element);

/* Enqueue one element at the front by copying elem_size bytes from element. */
enum cstd_status cstd_queue_pushfront(struct cstd_queue *queue, const void *element);

/* Dequeue one element from the front into out. */
enum cstd_status cstd_queue_popleft(struct cstd_queue *queue, void *out);

/* Remove one element from the back into out. */
enum cstd_status cstd_queue_popback(struct cstd_queue *queue, void *out);

/* Release owned storage and reset queue to an empty state. */
enum cstd_status cstd_queue_free(struct cstd_queue *queue);

/* Return the number of stored elements. */
size_t cstd_queue_size(const struct cstd_queue *queue);

#endif
