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
#include "vstd/datastruct/iterator.h"
#include "vstd/datastruct/vector.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

vs_iterator vs_iterator_from_callback(void *context, vs_iterator_next_fn next) {
    vs_iterator out;

    VSTD_ASSERT(next != NULL, "fatal: vs_iterator_from_callback invalid arguments");

    out.context = context;
    out.next = next;
    out.size_hint = 0;
    return out;
}

vs_iterator vs_iterator_from_state(vs_iterator_next_fn next) {
    vs_iterator out;

    VSTD_ASSERT(next != NULL, "fatal: vs_iterator_from_state invalid arguments");

    out.context = NULL;
    out.next = next;
    out.size_hint = 0;
    return out;
}

void *vs_iterator_state(vs_iterator *iter) {
    VSTD_ASSERT(iter != NULL, "fatal: vs_iterator_state invalid arguments");

    return iter->state.bytes;
}

void vs_iterator_set_size_hint(vs_iterator *iter, size_t size_hint) {
    VSTD_ASSERT(iter != NULL, "fatal: vs_iterator_set_size_hint invalid arguments");

    iter->size_hint = size_hint;
}

size_t vs_iterator_size_hint(const vs_iterator *iter) {
    VSTD_ASSERT(iter != NULL, "fatal: vs_iterator_size_hint invalid arguments");

    return iter->size_hint;
}

const void *vs_iterator_next(vs_iterator *iter) {
    VSTD_ASSERT(iter != NULL, "fatal: vs_iterator_next invalid arguments");
    VSTD_ASSERT(iter->next != NULL, "fatal: vs_iterator_next invalid arguments");

    const void *item = iter->next(iter->context != NULL ? iter->context : iter->state.bytes);
    if (item != NULL && iter->size_hint > 0) {
        iter->size_hint -= 1;
    }
    return item;
}

vs_status vs_iterator_collect(
    vs_iterator *source,
    size_t elem_size,
    vs_allocator *allocator,
    vs_vector **out
) {
    VSTD_ASSERT(source != NULL, "fatal: vs_iterator_collect invalid arguments");
    VSTD_ASSERT(elem_size > 0, "fatal: vs_iterator_collect invalid arguments");
    VSTD_ASSERT(out != NULL, "fatal: vs_iterator_collect invalid arguments");

    *out = NULL;

    vs_vector *vector = NULL;
    vs_status status = vs_vector_create(elem_size, allocator, &vector);
    if (status != VS_STATUS_OK) {
        return status;
    }
    if (source->size_hint > 0) {
        status = vs_vector_reserve(vector, source->size_hint);
        if (status != VS_STATUS_OK) {
            vs_vector_destroy(vector);
            return status;
        }
    }
    const void *item;
    while ((item = vs_iterator_next(source)) != NULL) {
        status = vs_vector_push(vector, item);
        if (status != VS_STATUS_OK) {
            vs_vector_destroy(vector);
            return status;
        }
    }

    *out = vector;
    return VS_STATUS_OK;
}

vs_status vs_iterator_collect_map(
    vs_iterator *source,
    size_t dst_elem_size,
    vs_iterator_map_into_fn map,
    void *context,
    vs_allocator *allocator,
    vs_vector **out
) {
    VSTD_ASSERT(source != NULL, "fatal: vs_iterator_collect_map invalid arguments");
    VSTD_ASSERT(dst_elem_size > 0, "fatal: vs_iterator_collect_map invalid arguments");
    VSTD_ASSERT(map != NULL, "fatal: vs_iterator_collect_map invalid arguments");
    VSTD_ASSERT(out != NULL, "fatal: vs_iterator_collect_map invalid arguments");

    *out = NULL;

    vs_vector *vector = NULL;
    vs_status status = vs_vector_create(dst_elem_size, allocator, &vector);
    if (status != VS_STATUS_OK) {
        return status;
    }
    if (source->size_hint > 0) {
        status = vs_vector_reserve(vector, source->size_hint);
        if (status != VS_STATUS_OK) {
            vs_vector_destroy(vector);
            return status;
        }
    }
    void *dst = NULL;
    status = vs_alloc(allocator, dst_elem_size, &dst);
    if (status != VS_STATUS_OK) {
        vs_vector_destroy(vector);
        return status;
    }

    const void *item;
    while ((item = vs_iterator_next(source)) != NULL) {
        map(context, item, dst);
        status = vs_vector_push(vector, dst);
        if (status != VS_STATUS_OK) {
            vs_dealloc(allocator, dst);
            vs_vector_destroy(vector);
            return status;
        }
    }
    vs_dealloc(allocator, dst);

    *out = vector;
    return VS_STATUS_OK;
}
