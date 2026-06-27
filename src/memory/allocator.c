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

#include <stdlib.h>

#include "vstd/assert.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

status alloc(allocator *allocator, size_t size, void **out) {
    ASSERT(out != NULL, "fatal: alloc invalid arguments");

    void *ptr = NULL;
    if (allocator == NULL || allocator->alloc == NULL) {
        ptr = malloc(size);
    } else {
        ptr = allocator->alloc(allocator->ctx, size);
    }

    if (ptr == NULL) {
        *out = NULL;
        return STATUS_NO_MEMORY;
    }

    *out = ptr;
    return STATUS_OK;
}

status resize(allocator *allocator, void *ptr, size_t size, void **out) {
    ASSERT(out != NULL, "fatal: resize invalid arguments");

    if (size == 0) {
        dealloc(allocator, ptr);
        *out = NULL;
        return STATUS_OK;
    }

    void *new_ptr = NULL;
    if (allocator == NULL || allocator->realloc == NULL) {
        new_ptr = realloc(ptr, size);
    } else {
        new_ptr = allocator->realloc(allocator->ctx, ptr, size);
    }

    if (new_ptr == NULL) {
        *out = NULL;
        return STATUS_NO_MEMORY;
    }

    *out = new_ptr;
    return STATUS_OK;
}

void dealloc(allocator *allocator, void *ptr) {
    if (allocator == NULL) {
        free(ptr);
        return;
    }

    if ((allocator->features & ALLOCATOR_FEATURE_DEALLOC) == 0) {
        return;
    }

    ASSERT(allocator->dealloc != NULL, "fatal: dealloc invalid arguments");
    allocator->dealloc(allocator->ctx, ptr);
}
