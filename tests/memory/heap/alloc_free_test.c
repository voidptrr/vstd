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
#include "vstd/memory/allocators/general_heap.h"

int main(void) {
    vs_heap *heap = vs_heap_create(2048);
    size_t before;
    size_t after;
    int *a;
    int *b;

    before = vs_heap_available(heap);

    a = (int *)vs_heap_alloc(heap, sizeof(int));
    b = (int *)vs_heap_alloc(heap, sizeof(int));
    VS_ASSERT_PTR_NOT_NULL(a);
    VS_ASSERT_PTR_NOT_NULL(b);

    after = vs_heap_available(heap);
    VS_ASSERT(after < before);

    vs_heap_dealloc(heap, a);
    vs_heap_dealloc(heap, b);
    vs_heap_destroy(heap);
    return 0;
}
