#ifndef CK_DATASTRUCT_DEQUE_H
#define CK_DATASTRUCT_DEQUE_H

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
typedef struct ck_deque ck_deque;

/* Initialize a deque with element size elem_size. */
ck_deque *ck_deque_init(size_t elem_size, ck_allocator *allocator);

/* Enqueue one element by copying elem_size bytes from element. */
void ck_deque_push(ck_deque *deque, const void *element);

/* Enqueue one element at the front by copying elem_size bytes from element. */
void ck_deque_pushfront(ck_deque *deque, const void *element);

/* Dequeue and return the front element pointer, or NULL when empty. */
void *ck_deque_popleft(ck_deque *deque);

/* Remove and return the back element pointer, or NULL when empty. */
void *ck_deque_popback(ck_deque *deque);

/* Return the front element pointer without removing it, or NULL when empty. */
const void *ck_deque_peekleft(const ck_deque *deque);

/* Return the back element pointer without removing it, or NULL when empty. */
const void *ck_deque_peekback(const ck_deque *deque);

/* Return the number of stored elements. */
size_t ck_deque_size(const ck_deque *deque);

/* Deinitialize and release owned storage. */
void ck_deque_deinit(ck_deque *deque);

#endif
