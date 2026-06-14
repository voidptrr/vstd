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

#include "ckit/testing.h"
#include "ckit/memory/allocators/arena.h"
#include "ckit/memory/allocators/allocator.h"

int main(void) {
    ck_arena *arena = ck_arena_create(128);
    ck_allocator allocator = ck_arena_allocator(arena);

    CK_TEST_ASSERT_PTR_EQ(allocator.ctx, arena);
    CK_TEST_ASSERT(allocator.alloc != NULL);
    CK_TEST_ASSERT(allocator.realloc != NULL);
    CK_TEST_ASSERT(allocator.dealloc == NULL);
    CK_TEST_ASSERT(allocator.features ==
                   (CK_ALLOCATOR_FEATURE_REALLOC | CK_ALLOCATOR_FEATURE_RESET));

    CK_TEST_ASSERT(ck_arena_capacity(arena) >= 128);
    CK_TEST_ASSERT_EQ(ck_arena_used(arena), 0);

    ck_arena_destroy(arena);
    return 0;
}
