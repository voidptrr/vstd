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

#include <stdint.h>

#include "vstd/assert.h"
#include "vstd/memory/allocators/arena.h"
#include "vstd/memory/utils.h"

int main(void) {
    vs_arena *arena = vs_arena_create(128);
    void *first;
    void *second;

    first = vs_arena_alloc(arena, 8);
    second = vs_arena_alloc(arena, 8);
    VS_ASSERT_PTR_NOT_NULL(first);
    VS_ASSERT_PTR_NOT_NULL(second);
    VS_ASSERT_PTR_NE(first, second);

    VS_ASSERT_EQ((uintptr_t)first % VS_MEMORY_ALIGN, 0);
    VS_ASSERT_EQ((uintptr_t)second % VS_MEMORY_ALIGN, 0);

    VS_ASSERT_PTR_NULL(vs_arena_alloc(arena, vs_arena_capacity(arena)));

    vs_arena_destroy(arena);
    return 0;
}
