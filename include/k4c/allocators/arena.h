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

#ifndef K4C_ARENA_H
#define K4C_ARENA_H

#include <stddef.h>

#include "k4c/allocators/allocator.h"
#include "k4c/error.h"

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
 * - bump offset forward on k4c_alloc
 * - individual k4c_dealloc is unsupported
 * - reset releases all allocations together
 * - small internal allocation headers are stored before payloads so realloc can
 *   preserve existing bytes
 */
typedef struct k4c_arena k4c_arena;

/*
 * Create an k4c_arena with `capacity` bytes of internal storage.
 */
k4c_status k4c_arena_create(size_t capacity, k4c_arena **out);

/*
 * Return the generic k4c_allocator view for this k4c_arena.
 */
k4c_allocator k4c_arena_allocator_view(k4c_arena *k4c_arena);

/*
 * Release all k4c_arena allocations while keeping the backing buffer.
 */
void k4c_arena_reset(k4c_arena *k4c_arena);

/* Total managed bytes. */
size_t k4c_arena_capacity(const k4c_arena *k4c_arena);

/* Number of bytes currently consumed. */
size_t k4c_arena_used(const k4c_arena *k4c_arena);

/* Number of bytes still available. */
size_t k4c_arena_available(const k4c_arena *k4c_arena);

/*
 * Destroy and release k4c_arena resources.
 */
void k4c_arena_destroy(k4c_arena *k4c_arena);

#endif
