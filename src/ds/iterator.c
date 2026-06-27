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

#include <stddef.h>

#include "vstd/assert.h"
#include "vstd/ds/iterator.h"
#include "vstd/ds/vector.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

iterator iterator_from_callback(void *context, iterator_next_fn next) {
    iterator out;

    ASSERT(next != NULL, "fatal: iterator_from_callback invalid arguments");

    out.context = context;
    out.next = next;
    out.size_hint = 0;
    return out;
}

iterator iterator_from_state(iterator_next_fn next) {
    iterator out;

    ASSERT(next != NULL, "fatal: iterator_from_state invalid arguments");

    out.context = NULL;
    out.next = next;
    out.size_hint = 0;
    return out;
}

void *iterator_state(iterator *iter) {
    ASSERT(iter != NULL, "fatal: iterator_state invalid arguments");

    return iter->state.bytes;
}

void iterator_set_size_hint(iterator *iter, size_t size_hint) {
    ASSERT(iter != NULL, "fatal: iterator_set_size_hint invalid arguments");

    iter->size_hint = size_hint;
}

size_t iterator_size_hint(const iterator *iter) {
    ASSERT(iter != NULL, "fatal: iterator_size_hint invalid arguments");

    return iter->size_hint;
}

const void *iterator_next(iterator *iter) {
    ASSERT(iter != NULL, "fatal: iterator_next invalid arguments");
    ASSERT(iter->next != NULL, "fatal: iterator_next invalid arguments");

    const void *item = iter->next(iter->context != NULL ? iter->context : iter->state.bytes);
    if (item != NULL && iter->size_hint > 0) {
        iter->size_hint -= 1;
    }
    return item;
}

status iterator_collect(iterator *source, size_t elem_size, allocator *allocator, vector **out) {
    ASSERT(source != NULL, "fatal: iterator_collect invalid arguments");
    ASSERT(elem_size > 0, "fatal: iterator_collect invalid arguments");
    ASSERT(out != NULL, "fatal: iterator_collect invalid arguments");

    *out = NULL;

    vector *vector = NULL;
    status st = vector_create(elem_size, allocator, &vector);
    if (st != STATUS_OK) {
        return st;
    }
    if (source->size_hint > 0) {
        st = vector_reserve(vector, source->size_hint);
        if (st != STATUS_OK) {
            vector_destroy(vector);
            return st;
        }
    }
    const void *item;
    while ((item = iterator_next(source)) != NULL) {
        st = vector_push(vector, item);
        if (st != STATUS_OK) {
            vector_destroy(vector);
            return st;
        }
    }

    *out = vector;
    return STATUS_OK;
}

status iterator_collect_map(
    iterator *source,
    size_t dst_elem_size,
    iterator_map_into_fn map,
    void *context,
    allocator *allocator,
    vector **out
) {
    ASSERT(source != NULL, "fatal: iterator_collect_map invalid arguments");
    ASSERT(dst_elem_size > 0, "fatal: iterator_collect_map invalid arguments");
    ASSERT(map != NULL, "fatal: iterator_collect_map invalid arguments");
    ASSERT(out != NULL, "fatal: iterator_collect_map invalid arguments");

    *out = NULL;

    vector *vector = NULL;
    status st = vector_create(dst_elem_size, allocator, &vector);
    if (st != STATUS_OK) {
        return st;
    }
    if (source->size_hint > 0) {
        st = vector_reserve(vector, source->size_hint);
        if (st != STATUS_OK) {
            vector_destroy(vector);
            return st;
        }
    }
    void *dst = NULL;
    st = alloc(allocator, dst_elem_size, &dst);
    if (st != STATUS_OK) {
        vector_destroy(vector);
        return st;
    }

    const void *item;
    while ((item = iterator_next(source)) != NULL) {
        map(context, item, dst);
        st = vector_push(vector, dst);
        if (st != STATUS_OK) {
            dealloc(allocator, dst);
            vector_destroy(vector);
            return st;
        }
    }
    dealloc(allocator, dst);

    *out = vector;
    return STATUS_OK;
}
