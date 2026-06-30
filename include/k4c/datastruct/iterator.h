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

#ifndef K4C_ITERATOR_H
#define K4C_ITERATOR_H

#include <stddef.h>

#define K4C_ITERATOR_STATE_SIZE 64

#define K4C_ITER_NEXT_AS(type, iter) ((const type *)k4c_iterator_next((iter)))

#define k4c_iterator_for_each(type, item, iter) \
    for (const type *(item) = K4C_ITER_NEXT_AS(type, (iter)); (item) != NULL; \
         (item) = K4C_ITER_NEXT_AS(type, (iter)))

/*
 * Generic k4c_iterator over caller-owned data.
 *
 * Iterator constructors return fully initialized k4c_iterator values.
 * k4c_iterator_next returns the next item pointer, or NULL when exhausted.
 */
typedef const void *(*k4c_iterator_next_fn)(void *context);

typedef union k4c_iterator_storage {
    max_align_t align;
    unsigned char bytes[K4C_ITERATOR_STATE_SIZE];
} k4c_iterator_storage;

typedef struct k4c_iterator {
    void *context;
    k4c_iterator_next_fn next;
    size_t size_hint;
    k4c_iterator_storage state;
} k4c_iterator;

/* Return an k4c_iterator backed by caller-owned context and a next callback. */
k4c_iterator k4c_iterator_from_callback(void *context, k4c_iterator_next_fn next);

/* Return an k4c_iterator backed by its internal state storage and a next callback. */
k4c_iterator k4c_iterator_from_state(k4c_iterator_next_fn next);

/* Return iter's internal state storage. */
void *k4c_iterator_state(k4c_iterator *iter);

/* Set a conservative count of remaining items, used by collectors to reserve storage. */
void k4c_iterator_set_size_hint(k4c_iterator *iter, size_t size_hint);

/* Return a conservative count of remaining items when known, or zero otherwise. */
size_t k4c_iterator_size_hint(const k4c_iterator *iter);

/* Return the next item from iter, or NULL when exhausted. */
const void *k4c_iterator_next(k4c_iterator *iter);

#endif
