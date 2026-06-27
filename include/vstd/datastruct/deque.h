/*
 * MIT License
 *
 * Copyright (c) 2026 Tommaso Bruno
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef VSTD_DEQUE_H
#define VSTD_DEQUE_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/datastruct/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

#define vs_deque_for_each(type, item, deque) \
    for (vs_iterator item##_vs_iter__ = vs_deque_get_iterator((deque)); \
         item##_vs_iter__.next != NULL; \
         item##_vs_iter__.next = NULL) \
        vs_iterator_for_each(type, item, &item##_vs_iter__)

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
typedef struct vs_deque vs_deque;

/* Create a deque with element size elem_size. */
VS_NODISCARD vs_status vs_deque_create(size_t elem_size, vs_allocator *allocator, vs_deque **out);

/* Enqueue one element by copying elem_size bytes from element. */
VS_NODISCARD vs_status vs_deque_push(vs_deque *deque, const void *element);

/* Enqueue one element at the front by copying elem_size bytes from element. */
VS_NODISCARD vs_status vs_deque_pushfront(vs_deque *deque, const void *element);

/* Dequeue and return the front element pointer, or NULL when empty. */
void *vs_deque_popleft(vs_deque *deque);

/* Remove and return the back element pointer, or NULL when empty. */
void *vs_deque_popback(vs_deque *deque);

/* Return the front element pointer without removing it, or NULL when empty. */
const void *vs_deque_peekleft(const vs_deque *deque);

/* Return the back element pointer without removing it, or NULL when empty. */
const void *vs_deque_peekback(const vs_deque *deque);

/* Return the number of stored elements. */
size_t vs_deque_size(const vs_deque *deque);

/* Return an iterator over deque from front to back. */
vs_iterator vs_deque_get_iterator(const vs_deque *deque);

/* Destroy and release owned storage. */
void vs_deque_destroy(vs_deque *deque);

#endif
