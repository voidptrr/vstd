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

#include "k4c/assert.h"
#include "k4c/ds/iterator.h"

k4c_iterator k4c_iterator_from_callback(void *context, k4c_iterator_next_fn next) {
    k4c_iterator out;

    K4C_ASSERT(next != NULL, "fatal: k4c_iterator_from_callback invalid arguments");

    out.context = context;
    out.next = next;
    out.size_hint = 0;
    return out;
}

k4c_iterator k4c_iterator_from_state(k4c_iterator_next_fn next) {
    k4c_iterator out;

    K4C_ASSERT(next != NULL, "fatal: k4c_iterator_from_state invalid arguments");

    out.context = NULL;
    out.next = next;
    out.size_hint = 0;
    return out;
}

void *k4c_iterator_state(k4c_iterator *iter) {
    K4C_ASSERT(iter != NULL, "fatal: k4c_iterator_state invalid arguments");

    return iter->state.bytes;
}

void k4c_iterator_set_size_hint(k4c_iterator *iter, size_t size_hint) {
    K4C_ASSERT(iter != NULL, "fatal: k4c_iterator_set_size_hint invalid arguments");

    iter->size_hint = size_hint;
}

size_t k4c_iterator_size_hint(const k4c_iterator *iter) {
    K4C_ASSERT(iter != NULL, "fatal: k4c_iterator_size_hint invalid arguments");

    return iter->size_hint;
}

const void *k4c_iterator_next(k4c_iterator *iter) {
    K4C_ASSERT(iter != NULL, "fatal: k4c_iterator_next invalid arguments");
    K4C_ASSERT(iter->next != NULL, "fatal: k4c_iterator_next invalid arguments");

    const void *item = iter->next(iter->context != NULL ? iter->context : iter->state.bytes);
    if (item != NULL && iter->size_hint > 0) {
        iter->size_hint -= 1;
    }
    return item;
}
