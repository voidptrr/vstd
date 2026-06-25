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

#ifndef VSTD_ITERATOR_H
#define VSTD_ITERATOR_H

#include <stddef.h>

/*
 * Generic iterator over caller-owned data.
 *
 * Iterator constructors return fully initialized iterator values.
 * vs_iterator_next returns the next item pointer, or NULL when exhausted.
 */
typedef const void *(*vs_iterator_next_fn)(void *context);
typedef void (*vs_iterator_map_into_fn)(void *context, const void *src, void *dst);

typedef struct vs_allocator vs_allocator;
typedef struct vs_vector vs_vector;

#define VS_ITERATOR_STATE_SIZE 64

typedef union vs_iterator_storage {
    max_align_t align;
    unsigned char bytes[VS_ITERATOR_STATE_SIZE];
} vs_iterator_storage;

typedef struct vs_iterator {
    void *context;
    vs_iterator_next_fn next;
    size_t size_hint;
    vs_iterator_storage state;
} vs_iterator;

#define VS_ITER_NEXT_AS(type, iter) ((const type *)vs_iterator_next((iter)))

#define VS_ITERATOR_FOR_EACH(type, item, iter) \
    for (const type *(item) = VS_ITER_NEXT_AS(type, (iter)); (item) != NULL; \
         (item) = VS_ITER_NEXT_AS(type, (iter)))

/* Return an iterator backed by caller-owned context and a next callback. */
vs_iterator vs_iterator_from_callback(void *context, vs_iterator_next_fn next);

/* Return an iterator backed by its internal state storage and a next callback. */
vs_iterator vs_iterator_from_state(vs_iterator_next_fn next);

/* Return iter's internal state storage. */
void *vs_iterator_state(vs_iterator *iter);

/* Set a conservative count of remaining items, used by collectors to reserve storage. */
void vs_iterator_set_size_hint(vs_iterator *iter, size_t size_hint);

/* Return a conservative count of remaining items when known, or zero otherwise. */
size_t vs_iterator_size_hint(const vs_iterator *iter);

/* Return the next item from iter, or NULL when exhausted. */
const void *vs_iterator_next(vs_iterator *iter);

/* Collect remaining source items by copying elem_size bytes into a new vector. */
vs_vector *vs_iterator_collect(vs_iterator *source, size_t elem_size, vs_allocator *allocator);

/* Map each remaining source item into dst_elem_size bytes and collect into a new vector. */
vs_vector *vs_iterator_collect_map(
    vs_iterator *source,
    size_t dst_elem_size,
    vs_iterator_map_into_fn map,
    void *context,
    vs_allocator *allocator
);

#endif
