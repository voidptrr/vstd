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

#ifndef CK_MEMORY_ARENA_H
#define CK_MEMORY_ARENA_H

#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Arena memory model:
 *
 * +----------------------+----------------------+
 * | used bytes           | available bytes      |
 * +----------------------+----------------------+
 *                        ^
 *                        current allocation offset
 *
 * Allocation strategy:
 * - bump offset forward on alloc
 * - individual dealloc is unsupported
 * - reset releases all allocations together
 * - small internal allocation headers are stored before payloads so realloc can
 *   preserve existing bytes
 */
typedef struct ck_arena ck_arena;

/*
 * Create an arena with `capacity` bytes of internal storage.
 */
ck_arena *ck_arena_create(size_t capacity);

/*
 * Return an allocator adapter that routes through this arena.
 */
ck_allocator ck_arena_allocator(ck_arena *arena);

/*
 * Allocate `size` bytes from arena.
 * Returns NULL when the arena does not have enough available space.
 */
void *ck_arena_alloc(ck_arena *arena, size_t size);

/*
 * Grow an arena allocation.
 * NULL ptr allocates. Size 0 with an existing ptr returns NULL.
 * Shrinking an existing allocation is invalid.
 */
void *ck_arena_realloc(ck_arena *arena, void *ptr, size_t size);

/*
 * Release all arena allocations while keeping the backing buffer.
 */
void ck_arena_reset(ck_arena *arena);

/* Total managed bytes. */
size_t ck_arena_capacity(const ck_arena *arena);

/* Number of bytes currently consumed. */
size_t ck_arena_used(const ck_arena *arena);

/* Number of bytes still available. */
size_t ck_arena_available(const ck_arena *arena);

/*
 * Destroy and release arena resources.
 */
void ck_arena_destroy(ck_arena *arena);

#endif
