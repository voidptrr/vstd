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

#include "vstd/memory/arena.h"
#include "vstd/memory/utils.h"
#include "vstd/testing.h"

int main(void) {
    vs_arena *arena = vs_arena_create(256);
    size_t grown_size = VS_MEMORY_ALIGN * 2;

    uint64_t *value = (uint64_t *)vs_arena_alloc(arena, sizeof(uint64_t));
    if (vs_test_not_null(value) != 0) {
        return 1;
    }
    *value = 42;

    uint64_t *grown = (uint64_t *)vs_arena_realloc(arena, value, grown_size);
    if (vs_test_not_null(grown) != 0) {
        return 1;
    }
    if (vs_test_not_equal_ptr(grown, value) != 0) {
        return 1;
    }
    if (vs_test_equal(grown[0], 42) != 0) {
        return 1;
    }

    if (vs_test_equal_ptr(vs_arena_realloc(arena, grown, grown_size), grown) != 0) {
        return 1;
    }

    vs_arena_destroy(arena);
    return 0;
}
