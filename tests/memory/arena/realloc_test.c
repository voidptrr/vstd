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
#include <stdint.h>

#include "ckit/testing.h"
#include "ckit/memory/allocators/arena.h"
#include "memory/utils.h"

int main(void) {
    ck_arena *arena = ck_arena_init(256);
    uint64_t *value;
    uint64_t *grown;
    size_t grown_size = CK_MEMORY_ALIGN * 2;

    value = (uint64_t *)ck_arena_alloc(arena, sizeof(uint64_t));
    CK_TEST_ASSERT_PTR_NOT_NULL(value);
    *value = 42;

    grown = (uint64_t *)ck_arena_realloc(arena, value, grown_size);
    CK_TEST_ASSERT_PTR_NOT_NULL(grown);
    CK_TEST_ASSERT_PTR_NE(grown, value);
    CK_TEST_ASSERT_EQ(grown[0], 42);

    CK_TEST_ASSERT_PTR_EQ(ck_arena_realloc(arena, grown, grown_size), grown);

    ck_arena_deinit(arena);
    return 0;
}
