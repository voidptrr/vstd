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

#include <stdio.h>

#include "ckit/memory/allocators/general_heap.h"

int main(void) {
    int status = 0;
    ck_heap *heap = ck_heap_init(2048);
    size_t before;
    size_t after;
    int *a;
    int *b;

    before = ck_heap_available(heap);

    a = (int *)ck_heap_alloc(heap, sizeof(int));
    b = (int *)ck_heap_alloc(heap, sizeof(int));
    if (a == NULL || b == NULL) {
        fprintf(stderr, "heap alloc should return pointers\n");
        status = 1;
        goto cleanup;
    }

    after = ck_heap_available(heap);
    if (after >= before) {
        fprintf(stderr, "available bytes should decrease after alloc\n");
        status = 1;
        goto cleanup;
    }

    ck_heap_dealloc(heap, a);
    ck_heap_dealloc(heap, b);
cleanup:
    ck_heap_deinit(heap);
    return status;
}
