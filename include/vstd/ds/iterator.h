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

#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>

#include "vstd/error.h"

#define ITERATOR_STATE_SIZE 64

#define ITER_NEXT_AS(type, iter) ((const type *)iterator_next((iter)))

#define iterator_for_each(type, item, iter) \
    for (const type *(item) = ITER_NEXT_AS(type, (iter)); (item) != NULL; \
         (item) = ITER_NEXT_AS(type, (iter)))

/*
 * Generic iterator over caller-owned data.
 *
 * Iterator constructors return fully initialized iterator values.
 * iterator_next returns the next item pointer, or NULL when exhausted.
 */
typedef const void *(*iterator_next_fn)(void *context);
typedef void (*iterator_map_into_fn)(void *context, const void *src, void *dst);

typedef struct allocator allocator;
typedef struct vector vector;

typedef union iterator_storage {
    max_align_t align;
    unsigned char bytes[ITERATOR_STATE_SIZE];
} iterator_storage;

typedef struct iterator {
    void *context;
    iterator_next_fn next;
    size_t size_hint;
    iterator_storage state;
} iterator;

/* Return an iterator backed by caller-owned context and a next callback. */
iterator iterator_from_callback(void *context, iterator_next_fn next);

/* Return an iterator backed by its internal state storage and a next callback. */
iterator iterator_from_state(iterator_next_fn next);

/* Return iter's internal state storage. */
void *iterator_state(iterator *iter);

/* Set a conservative count of remaining items, used by collectors to reserve storage. */
void iterator_set_size_hint(iterator *iter, size_t size_hint);

/* Return a conservative count of remaining items when known, or zero otherwise. */
size_t iterator_size_hint(const iterator *iter);

/* Return the next item from iter, or NULL when exhausted. */
const void *iterator_next(iterator *iter);

/* Collect remaining source items by copying elem_size bytes into a new vector. */
NODISCARD status
iterator_collect(iterator *source, size_t elem_size, allocator *allocator, vector **out);

/* Map each remaining source item into dst_elem_size bytes and collect into a new vector. */
NODISCARD status iterator_collect_map(
    iterator *source,
    size_t dst_elem_size,
    iterator_map_into_fn map,
    void *context,
    allocator *allocator,
    vector **out
);

#endif
