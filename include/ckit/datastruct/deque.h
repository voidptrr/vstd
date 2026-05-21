#ifndef CKIT_DATASTRUCT_DEQUE_H
#define CKIT_DATASTRUCT_DEQUE_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Opaque generic double-ended queue backed by circular storage.
 *
 * Logical view:
 *
 *              popleft                 popback
 *                 |                       |
 *                 v                       v
 *          +------+------+------+ ... +------+
 *   front  |  T   |  T   |  T   |     |  T   |  back
 *          +------+------+------+ ... +------+
 *                 ^                       ^
 *                 |                       |
 *             pushfront                 push
 *
 * Storage may wrap internally, but callers observe a normal front-to-back
 * sequence.
 */
typedef struct ckit_deque ckit_deque;

/* Create a deque with element size elem_size. */
ckit_deque *ckit_deque_init(size_t elem_size, ckit_allocator *allocator);

/* Enqueue one element by copying elem_size bytes from element. */
void ckit_deque_push(ckit_deque *deque, const void *element);

/* Enqueue one element at the front by copying elem_size bytes from element. */
void ckit_deque_pushfront(ckit_deque *deque, const void *element);

/* Dequeue and return the front element pointer, or NULL when empty. */
void *ckit_deque_popleft(ckit_deque *deque);

/* Remove and return the back element pointer, or NULL when empty. */
void *ckit_deque_popback(ckit_deque *deque);

/* Return the front element pointer without removing it, or NULL when empty. */
const void *ckit_deque_peekleft(const ckit_deque *deque);

/* Return the back element pointer without removing it, or NULL when empty. */
const void *ckit_deque_peekback(const ckit_deque *deque);

/* Release owned storage and the deque handle. */
void ckit_deque_free(ckit_deque *deque);

/* Return the number of stored elements. */
size_t ckit_deque_size(const ckit_deque *deque);

/* Return whether the deque has zero elements. */
bool ckit_deque_is_empty(const ckit_deque *deque);

#endif
