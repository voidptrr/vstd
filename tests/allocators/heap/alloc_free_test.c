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

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/general_heap.h"
#include "k4c/error.h"
#include "k4c/testing.h"

int main(void) {
    k4c_heap *k4c_heap = NULL;
    if (k4c_test_equal(k4c_heap_create(2048, &k4c_heap), K4C_STATUS_OK)) {
        return 1;
    }
    size_t before = k4c_heap_available(k4c_heap);
    k4c_allocator allocator = k4c_heap_allocator_view(k4c_heap);

    int *a = NULL;
    int *b = NULL;
    if (k4c_test_status_ok(k4c_alloc(&allocator, sizeof(int), (void **)&a)) != 0) {
        return 1;
    }
    if (k4c_test_status_ok(k4c_alloc(&allocator, sizeof(int), (void **)&b)) != 0) {
        return 1;
    }
    if (k4c_test_not_null(a) != 0) {
        return 1;
    }
    if (k4c_test_not_null(b) != 0) {
        return 1;
    }

    size_t after = k4c_heap_available(k4c_heap);
    if (k4c_test_equal(after < before, true) != 0) {
        return 1;
    }

    k4c_dealloc(&allocator, a);
    k4c_dealloc(&allocator, b);
    k4c_heap_destroy(k4c_heap);
    return 0;
}
