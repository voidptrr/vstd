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

/* Create a deque with element size elem_size. */
ck_deque *ck_deque_create(size_t elem_size, ck_allocator *allocator);

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

/* Destroy and release owned storage. */
void ck_deque_destroy(ck_deque *deque);

#endif
