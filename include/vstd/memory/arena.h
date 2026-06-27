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

#ifndef VSTD_ARENA_H
#define VSTD_ARENA_H

#include <stddef.h>

#include "vstd/error.h"
#include "vstd/memory/allocator.h"

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
typedef struct vs_arena vs_arena;

/*
 * Create an arena with `capacity` bytes of internal storage.
 */
VS_NODISCARD vs_status vs_arena_create(size_t capacity, vs_arena **out);

/*
 * Return the generic allocator view owned by this arena.
 */
vs_allocator *vs_arena_allocator(vs_arena *arena);

/*
 * Allocate `size` bytes from arena.
 * Returns NULL when the arena does not have enough available space.
 */
void *vs_arena_alloc(vs_arena *arena, size_t size);

/*
 * Grow an arena allocation.
 * NULL ptr allocates. Size 0 with an existing ptr returns NULL.
 * Shrinking an existing allocation is invalid.
 */
void *vs_arena_realloc(vs_arena *arena, void *ptr, size_t size);

/*
 * Release all arena allocations while keeping the backing buffer.
 */
void vs_arena_reset(vs_arena *arena);

/* Total managed bytes. */
size_t vs_arena_capacity(const vs_arena *arena);

/* Number of bytes currently consumed. */
size_t vs_arena_used(const vs_arena *arena);

/* Number of bytes still available. */
size_t vs_arena_available(const vs_arena *arena);

/*
 * Destroy and release arena resources.
 */
void vs_arena_destroy(vs_arena *arena);

#endif
