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

#include "vstd/memory/allocators/allocator.h"
#include "vstd/memory/allocators/arena.h"
#include "vstd/testing.h"

int main(void) {
    vs_arena *arena = vs_arena_create(128);
    vs_allocator allocator = vs_arena_adapter(arena);

    VS_TEST_ASSERT_PTR_EQ(allocator.ctx, arena);
    VS_TEST_ASSERT(allocator.alloc != NULL);
    VS_TEST_ASSERT(allocator.realloc != NULL);
    VS_TEST_ASSERT(allocator.dealloc == NULL);
    VS_TEST_ASSERT(
        allocator.features == (VS_ALLOCATOR_FEATURE_REALLOC | VS_ALLOCATOR_FEATURE_RESET)
    );

    VS_TEST_ASSERT(vs_arena_capacity(arena) >= 128);
    VS_TEST_ASSERT_EQ(vs_arena_used(arena), 0);

    vs_arena_destroy(arena);
    return 0;
}
