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

#include <string.h>

#include "ckit/memory/allocators/general_heap.h"
#include "ckit/testing.h"

int main(void) {
    ck_heap *heap = ck_heap_create(4096);
    char *ptr;

    ptr = (char *)ck_heap_realloc(heap, NULL, 32);
    CK_TEST_ASSERT_PTR_NOT_NULL(ptr);

    memcpy(ptr, "hello", 6);
    ptr = (char *)ck_heap_realloc(heap, ptr, 128);
    CK_TEST_ASSERT_PTR_NOT_NULL(ptr);
    CK_TEST_ASSERT_EQ(memcmp(ptr, "hello", 6), 0);

    CK_TEST_ASSERT_PTR_NULL(ck_heap_realloc(heap, ptr, 0));

    ck_heap_destroy(heap);
    return 0;
}
