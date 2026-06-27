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

#include "vstd/error.h"
#include "vstd/memory/arena.h"
#include "vstd/memory/utils.h"
#include "vstd/testing.h"

int main(void) {
    arena *arena = NULL;
    if (test_equal(arena_create(128, &arena), STATUS_OK)) {
        return 1;
    }
    void *first = arena_alloc(arena, 8);
    void *second = arena_alloc(arena, 8);
    if (test_not_null(first) != 0) {
        return 1;
    }
    if (test_not_null(second) != 0) {
        return 1;
    }
    if (test_not_equal_ptr(first, second) != 0) {
        return 1;
    }

    if (test_equal((intmax_t)((uintptr_t)first % MEMORY_ALIGN), 0) != 0) {
        return 1;
    }
    if (test_equal((intmax_t)((uintptr_t)second % MEMORY_ALIGN), 0) != 0) {
        return 1;
    }

    if (test_null(arena_alloc(arena, arena_capacity(arena))) != 0) {
        return 1;
    }

    arena_destroy(arena);
    return 0;
}
