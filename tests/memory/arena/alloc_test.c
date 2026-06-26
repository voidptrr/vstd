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

#include "vstd/memory/arena.h"
#include "vstd/memory/utils.h"
#include "vstd/testing.h"

int main(void) {
    vs_arena *arena = vs_arena_create(128);
    void *first = vs_arena_alloc(arena, 8);
    void *second = vs_arena_alloc(arena, 8);
    if (vs_test_not_null(first) != 0) {
        return 1;
    }
    if (vs_test_not_null(second) != 0) {
        return 1;
    }
    if (vs_test_not_equal_ptr(first, second) != 0) {
        return 1;
    }

    if (vs_test_equal((intmax_t)((uintptr_t)first % VS_MEMORY_ALIGN), 0) != 0) {
        return 1;
    }
    if (vs_test_equal((intmax_t)((uintptr_t)second % VS_MEMORY_ALIGN), 0) != 0) {
        return 1;
    }

    if (vs_test_null(vs_arena_alloc(arena, vs_arena_capacity(arena))) != 0) {
        return 1;
    }

    vs_arena_destroy(arena);
    return 0;
}
