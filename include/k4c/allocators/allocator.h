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

#ifndef K4C_ALLOCATOR_H
#define K4C_ALLOCATOR_H

#include <stddef.h>

#include "k4c/error.h"

typedef void *(*k4c_alloc_fn)(void *ctx, size_t size);
typedef void *(*k4c_resize_fn)(void *ctx, void *ptr, size_t size);
typedef void (*k4c_dealloc_fn)(void *ctx, void *ptr);
typedef struct k4c_allocator_vtable k4c_allocator_vtable;

typedef enum k4c_allocator_features {
    K4C_ALLOCATOR_FEATURE_DEALLOC = 1 << 0,
    K4C_ALLOCATOR_FEATURE_REALLOC = 1 << 1,
    K4C_ALLOCATOR_FEATURE_RESET = 1 << 2,
} k4c_allocator_features;

/*
 * Generic k4c_allocator used by containers.
 * Owning create APIs capture the k4c_allocator pointer and reuse it for later
 * growth and destroy calls, so the k4c_allocator must outlive those objects.
 */
typedef struct k4c_allocator {
    /* User-supplied k4c_allocator state passed to vtable operations. */
    void *ctx;
    /* Bitmask of K4C_ALLOCATOR_FEATURE_* values supported by this k4c_allocator. */
    unsigned int features;
    /* Operation table used by generic allocation helpers. */
    const k4c_allocator_vtable *vtable;
} k4c_allocator;

struct k4c_allocator_vtable {
    /* Allocate size bytes and return pointer or NULL on failure. */
    k4c_alloc_fn alloc;
    /* Resize ptr to size bytes and return new pointer or NULL on failure. */
    k4c_resize_fn resize;
    /* Release a pointer previously returned by alloc/resize. */
    k4c_dealloc_fn dealloc;
};

/* Allocate size bytes through k4c_allocator vtable when provided, otherwise malloc. */
k4c_status k4c_alloc(k4c_allocator *k4c_allocator, size_t size, void **out);

/* Copy allocator when provided, otherwise return the default libc-backed allocator value. */
k4c_allocator k4c_allocator_copy(const k4c_allocator *k4c_allocator);

/* Resize ptr through k4c_allocator vtable when provided, otherwise realloc. */
k4c_status k4c_resize(k4c_allocator *k4c_allocator, void *ptr, size_t size, void **out);

/* Release ptr through k4c_allocator vtable when advertised, otherwise free for NULL
 * k4c_allocator. */
void k4c_dealloc(k4c_allocator *k4c_allocator, void *ptr);

#endif
