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

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/arena.h"
#include "k4c/error.h"
#include "k4c/testing.h"

int main(void) {
    k4c_arena *k4c_arena = NULL;
    if (k4c_test_equal(k4c_arena_create(128, &k4c_arena), K4C_STATUS_OK)) {
        return 1;
    }
    k4c_allocator k4c_allocator = k4c_arena_allocator_view(k4c_arena);

    if (k4c_test_equal_ptr(k4c_allocator.ctx, k4c_arena) != 0) {
        return 1;
    }
    if (k4c_allocator.vtable == NULL) {
        return 1;
    }
    if (k4c_test_equal(k4c_allocator.vtable->alloc != NULL, true) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_allocator.vtable->resize != NULL, true) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_allocator.vtable->dealloc == NULL, true) != 0) {
        return 1;
    }
    if (k4c_test_equal(
            k4c_allocator.features == (K4C_ALLOCATOR_FEATURE_REALLOC | K4C_ALLOCATOR_FEATURE_RESET),
            true
        )
        != 0) {
        return 1;
    }

    if (k4c_test_equal(k4c_arena_capacity(k4c_arena) >= 128, true) != 0) {
        return 1;
    }
    if (k4c_test_equal((intmax_t)k4c_arena_used(k4c_arena), 0) != 0) {
        return 1;
    }

    k4c_arena_destroy(k4c_arena);
    return 0;
}
