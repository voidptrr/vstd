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

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/arena.h"
#include "k4c/allocators/utils.h"
#include "k4c/error.h"
#include "k4c/testing.h"

int main(void) {
    k4c_arena *k4c_arena = NULL;
    if (k4c_test_equal(k4c_arena_create(256, &k4c_arena), K4C_STATUS_OK)) {
        return 1;
    }
    k4c_allocator allocator = k4c_arena_allocator_view(k4c_arena);
    size_t grown_size = K4C_MEMORY_ALIGN * 2;

    uint64_t *value = NULL;
    if (k4c_test_status_ok(k4c_alloc(&allocator, sizeof(uint64_t), (void **)&value)) != 0) {
        return 1;
    }
    if (k4c_test_not_null(value) != 0) {
        return 1;
    }
    *value = 42;

    uint64_t *grown = NULL;
    if (k4c_test_status_ok(k4c_resize(&allocator, value, grown_size, (void **)&grown)) != 0) {
        return 1;
    }
    if (k4c_test_not_null(grown) != 0) {
        return 1;
    }
    if (k4c_test_not_equal_ptr(grown, value) != 0) {
        return 1;
    }
    if (k4c_test_equal(grown[0], 42) != 0) {
        return 1;
    }

    uint64_t *same = NULL;
    if (k4c_test_status_ok(k4c_resize(&allocator, grown, grown_size, (void **)&same)) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(same, grown) != 0) {
        return 1;
    }

    k4c_arena_destroy(k4c_arena);
    return 0;
}
