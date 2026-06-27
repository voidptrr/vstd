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

#ifndef VSTD_ALLOCATOR_H
#define VSTD_ALLOCATOR_H

#include <stddef.h>

#include "vstd/error.h"

typedef void *(*vs_alloc_fn)(void *ctx, size_t size);
typedef void *(*vs_realloc_fn)(void *ctx, void *ptr, size_t size);
typedef void (*vs_dealloc_fn)(void *ctx, void *ptr);

typedef enum vs_allocator_features {
    VS_ALLOCATOR_FEATURE_DEALLOC = 1 << 0,
    VS_ALLOCATOR_FEATURE_REALLOC = 1 << 1,
    VS_ALLOCATOR_FEATURE_RESET = 1 << 2,
} vs_allocator_features;

/*
 * Generic allocator callback set used by containers.
 * Owning create APIs capture the allocator pointer and reuse it for later
 * growth and destroy calls, so the allocator must outlive those objects.
 */
typedef struct vs_allocator {
    /* User-supplied allocator state passed to all callbacks. */
    void *ctx;
    /* Bitmask of VS_ALLOCATOR_FEATURE_* values supported by this allocator. */
    unsigned int features;
    /* Allocate size bytes and return pointer or NULL on failure. */
    vs_alloc_fn alloc;
    /* Resize ptr to size bytes and return new pointer or NULL on failure. */
    vs_realloc_fn realloc;
    /* Release a pointer previously returned by alloc/realloc. */
    vs_dealloc_fn dealloc;
} vs_allocator;

/* Allocate size bytes through allocator->alloc when provided, otherwise malloc. */
VS_NODISCARD vs_status vs_alloc(vs_allocator *allocator, size_t size, void **out);

/* Resize ptr through allocator->realloc when provided, otherwise realloc. */
VS_NODISCARD vs_status vs_resize(vs_allocator *allocator, void *ptr, size_t size, void **out);

/* Release ptr through allocator->dealloc when advertised, otherwise free for NULL allocator. */
void vs_dealloc(vs_allocator *allocator, void *ptr);

#endif
