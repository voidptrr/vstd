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

#include <stdio.h>

#include "ckit/memory/allocators/arena.h"

int main(void) {
    int status = 0;
    ck_arena *arena = ck_arena_init(128);
    ck_allocator allocator = ck_arena_allocator(arena);

    if (allocator.ctx != arena || allocator.alloc == NULL || allocator.realloc == NULL ||
        allocator.dealloc != NULL ||
        allocator.features != (CK_ALLOCATOR_FEATURE_REALLOC | CK_ALLOCATOR_FEATURE_RESET)) {
        fprintf(stderr, "arena allocator should return allocator adapter\n");
        status = 1;
        goto cleanup;
    }

    if (ck_arena_capacity(arena) < 128 || ck_arena_used(arena) != 0) {
        fprintf(stderr, "arena init should set capacity and empty offset\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_arena_deinit(arena);
    return status;
}
