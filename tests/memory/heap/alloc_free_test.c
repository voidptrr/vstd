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

#include "vstd/error.h"
#include "vstd/memory/general_heap.h"
#include "vstd/testing.h"

int main(void) {
    heap *heap = NULL;
    if (test_equal(heap_create(2048, &heap), STATUS_OK)) {
        return 1;
    }
    size_t before = heap_available(heap);

    int *a = (int *)heap_alloc(heap, sizeof(int));
    int *b = (int *)heap_alloc(heap, sizeof(int));
    if (test_not_null(a) != 0) {
        return 1;
    }
    if (test_not_null(b) != 0) {
        return 1;
    }

    size_t after = heap_available(heap);
    if (test_equal(after < before, true) != 0) {
        return 1;
    }

    heap_dealloc(heap, a);
    heap_dealloc(heap, b);
    heap_destroy(heap);
    return 0;
}
