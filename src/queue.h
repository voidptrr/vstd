#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

enum queue_status { QUEUE_OK, QUEUE_ERR_OOM, QUEUE_ERR_NULL, QUEUE_ERR_EMPTY };

/*
 * Generic FIFO queue backed by a circular buffer.
 * +----------------------+----------------------+----------------------+
 * | size (size_t)        | elem_size (size_t)   | capacity (size_t)    |
 * +----------------------+----------------------+----------------------+
 * | head (size_t)        | tail (size_t)        | buffer (void *)      |
 * +----------------------+----------------------+----------------------+
 */
struct queue {
    size_t size;
    size_t elem_size;
    size_t capacity;
    size_t head;
    size_t tail;
    void *buffer;
};

/*
 * Initialize a queue with element size elem_size.
 * Returns QUEUE_OK on success, QUEUE_ERR_NULL when queue is NULL,
 * or QUEUE_ERR_OOM when allocation fails.
 */
enum queue_status queue_init(struct queue *queue, size_t elem_size);

/*
 * Enqueue one element by copying elem_size bytes from element.
 * Returns QUEUE_OK, QUEUE_ERR_NULL for invalid pointers,
 * or QUEUE_ERR_OOM when growth allocation fails.
 */
enum queue_status queue_push(struct queue *queue, const void *element);

/*
 * Dequeue one element into out.
 * Returns QUEUE_OK, QUEUE_ERR_NULL for invalid pointers,
 * or QUEUE_ERR_EMPTY when queue has no elements.
 */
enum queue_status queue_popleft(struct queue *queue, void *out);

/*
 * Remove one element from the back into out.
 * Returns QUEUE_OK, QUEUE_ERR_NULL for invalid pointers,
 * or QUEUE_ERR_EMPTY when queue has no elements.
 */
enum queue_status queue_popback(struct queue *queue, void *out);

/*
 * Release owned storage and reset queue to an empty state.
 * Returns QUEUE_OK, or QUEUE_ERR_NULL when queue is NULL.
 */
enum queue_status queue_free(struct queue *queue);

/*
 * Return the number of stored elements.
 * Returns 0 when queue is NULL.
 */
size_t queue_size(struct queue *queue);

#endif
