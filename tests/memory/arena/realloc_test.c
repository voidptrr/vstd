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
#include <stdio.h>

#include "ckit/memory/allocators/arena.h"
#include "memory/utils.h"

int main(void) {
    int status = 0;
    ck_arena *arena = ck_arena_init(256);
    uint64_t *value;
    uint64_t *grown;
    size_t grown_size = CK_MEMORY_ALIGN * 2;

    value = (uint64_t *)ck_arena_alloc(arena, sizeof(uint64_t));
    if (value == NULL) {
        fprintf(stderr, "arena alloc should return memory\n");
        status = 1;
        goto cleanup;
    }
    *value = 42;

    grown = (uint64_t *)ck_arena_realloc(arena, value, grown_size);
    if (grown == NULL || grown == value || grown[0] != 42) {
        fprintf(stderr, "arena realloc should grow and preserve old bytes\n");
        status = 1;
        goto cleanup;
    }

    if (ck_arena_realloc(arena, grown, grown_size) != grown) {
        fprintf(stderr, "arena realloc with same size should return same pointer\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_arena_deinit(arena);
    return status;
}
