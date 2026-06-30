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

#include "k4c/allocators/allocator.h"
#include "k4c/assert.h"
#include "k4c/error.h"

k4c_status k4c_alloc(k4c_allocator *k4c_allocator, size_t size, void **out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_alloc invalid arguments");

    void *ptr = NULL;
    if (k4c_allocator == NULL || k4c_allocator->vtable == NULL
        || k4c_allocator->vtable->alloc == NULL) {
        ptr = malloc(size);
    } else {
        ptr = k4c_allocator->vtable->alloc(k4c_allocator->ctx, size);
    }

    if (ptr == NULL) {
        *out = NULL;
        return K4C_STATUS_NO_MEMORY;
    }

    *out = ptr;
    return K4C_STATUS_OK;
}

k4c_allocator k4c_allocator_copy(const k4c_allocator *k4c_allocator) {
    if (k4c_allocator == NULL) {
        return (struct k4c_allocator){0};
    }
    return *k4c_allocator;
}

k4c_status k4c_resize(k4c_allocator *k4c_allocator, void *ptr, size_t size, void **out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_resize invalid arguments");

    if (size == 0) {
        k4c_dealloc(k4c_allocator, ptr);
        *out = NULL;
        return K4C_STATUS_OK;
    }

    void *new_ptr = NULL;
    if (k4c_allocator == NULL || k4c_allocator->vtable == NULL
        || k4c_allocator->vtable->resize == NULL) {
        new_ptr = realloc(ptr, size);
    } else {
        new_ptr = k4c_allocator->vtable->resize(k4c_allocator->ctx, ptr, size);
    }

    if (new_ptr == NULL) {
        *out = NULL;
        return K4C_STATUS_NO_MEMORY;
    }

    *out = new_ptr;
    return K4C_STATUS_OK;
}

void k4c_dealloc(k4c_allocator *k4c_allocator, void *ptr) {
    if (k4c_allocator == NULL || k4c_allocator->vtable == NULL) {
        free(ptr);
        return;
    }

    if ((k4c_allocator->features & K4C_ALLOCATOR_FEATURE_DEALLOC) == 0) {
        return;
    }

    K4C_ASSERT(k4c_allocator->vtable != NULL, "fatal: k4c_dealloc invalid arguments");
    K4C_ASSERT(k4c_allocator->vtable->dealloc != NULL, "fatal: k4c_dealloc invalid arguments");
    k4c_allocator->vtable->dealloc(k4c_allocator->ctx, ptr);
}
