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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "mem/size.h"
#include "k4c/allocators/allocator.h"
#include "k4c/allocators/general_heap.h"
#include "k4c/allocators/utils.h"
#include "k4c/assert.h"
#include "k4c/datastruct/doubly_linked_list.h"
#include "k4c/error.h"

/* Header stored immediately before each k4c_heap-managed payload block. */
typedef struct k4c_heap_block {
    size_t size;
    k4c_doubly_linked_list_node node;
    bool is_free;
} k4c_heap_block;

struct k4c_heap {
    void *buffer;
    size_t capacity;
    k4c_doubly_linked_list *blocks;
};

static void *k4c_heap_vtable_alloc(void *ctx, size_t size);
static void k4c_heap_vtable_dealloc(void *ctx, void *ptr);
static void *k4c_heap_vtable_realloc(void *ctx, void *ptr, size_t size);

static const k4c_allocator_vtable k4c_heap_allocator_vtable = {
    .alloc = k4c_heap_vtable_alloc,
    .resize = k4c_heap_vtable_realloc,
    .dealloc = k4c_heap_vtable_dealloc,
};

static k4c_heap_block *k4c_heap_head(const k4c_heap *k4c_heap) {
    k4c_doubly_linked_list_node *head = k4c_doubly_linked_list_head(k4c_heap->blocks);
    if (head == NULL) {
        return NULL;
    }

    return K4C_CONTAINER_OF(head, k4c_heap_block, node);
}

static k4c_heap_block *k4c_heap_next(const k4c_heap_block *block) {
    if (block->node.next == NULL) {
        return NULL;
    }

    return K4C_CONTAINER_OF(block->node.next, k4c_heap_block, node);
}

static k4c_heap_block *k4c_heap_prev(const k4c_heap_block *block) {
    if (block->node.prev == NULL) {
        return NULL;
    }

    return K4C_CONTAINER_OF(block->node.prev, k4c_heap_block, node);
}

static void k4c_heap_split_block(k4c_heap *k4c_heap, k4c_heap_block *block, size_t size) {
    size_t remaining = block->size - size;

    if (remaining <= sizeof(k4c_heap_block) + K4C_MEMORY_ALIGN) {
        return;
    }

    k4c_heap_block *split = (k4c_heap_block *)((uint8_t *)block + sizeof(k4c_heap_block) + size);
    split->size = remaining - sizeof(k4c_heap_block);
    split->is_free = true;

    block->size = size;
    k4c_doubly_linked_list_insert_after(k4c_heap->blocks, &block->node, &split->node);
}

static k4c_heap_block *k4c_heap_coalesce(k4c_heap *k4c_heap, k4c_heap_block *block) {
    k4c_heap_block *next = k4c_heap_next(block);
    if (next != NULL && next->is_free) {
        block->size += sizeof(k4c_heap_block) + next->size;
        k4c_doubly_linked_list_remove(k4c_heap->blocks, &next->node);
    }

    k4c_heap_block *prev = k4c_heap_prev(block);
    if (prev != NULL && prev->is_free) {
        prev->size += sizeof(k4c_heap_block) + block->size;
        k4c_doubly_linked_list_remove(k4c_heap->blocks, &block->node);
        block = prev;
    }

    return block;
}

static void *k4c_heap_vtable_alloc(void *ctx, size_t size) {
    k4c_heap *k4c_heap = ctx;

    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_heap_alloc invalid arguments");
    K4C_ASSERT(k4c_heap->buffer != NULL, "fatal: k4c_heap_alloc invalid k4c_heap");
    K4C_ASSERT(k4c_heap->blocks != NULL, "fatal: k4c_heap_alloc invalid k4c_heap");
    K4C_ASSERT(size > 0, "fatal: k4c_heap_alloc invalid arguments");

    if (k4c_align_up_overflow(size, K4C_MEMORY_ALIGN, &size)) {
        return NULL;
    }

    k4c_heap_block *block = k4c_heap_head(k4c_heap);
    while (block != NULL) {
        if (block->is_free && block->size >= size) {
            k4c_heap_split_block(k4c_heap, block, size);
            block->is_free = false;
            return (uint8_t *)block + sizeof(k4c_heap_block);
        }
        block = k4c_heap_next(block);
    }

    return NULL;
}

static void k4c_heap_vtable_dealloc(void *ctx, void *ptr) {
    k4c_heap *k4c_heap = ctx;

    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_heap_dealloc invalid arguments");
    K4C_ASSERT(ptr != NULL, "fatal: k4c_heap_dealloc invalid arguments");
    K4C_ASSERT(k4c_heap->buffer != NULL, "fatal: k4c_heap_dealloc invalid k4c_heap");
    K4C_ASSERT(k4c_heap->blocks != NULL, "fatal: k4c_heap_dealloc invalid k4c_heap");

    uintptr_t start = (uintptr_t)k4c_heap->buffer;
    uintptr_t end = start + k4c_heap->capacity;
    uintptr_t value = (uintptr_t)ptr;
    if (value <= start || value >= end) {
        return;
    }

    k4c_heap_block *block = (k4c_heap_block *)((uint8_t *)ptr - sizeof(k4c_heap_block));
    block->is_free = true;
    k4c_heap_coalesce(k4c_heap, block);
}

static void *k4c_heap_vtable_realloc(void *ctx, void *ptr, size_t size) {
    k4c_heap *k4c_heap = ctx;

    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_heap_realloc invalid arguments");
    K4C_ASSERT(k4c_heap->buffer != NULL, "fatal: k4c_heap_realloc invalid k4c_heap");
    K4C_ASSERT(k4c_heap->blocks != NULL, "fatal: k4c_heap_realloc invalid k4c_heap");

    if (ptr == NULL) {
        return k4c_heap_vtable_alloc(ctx, size);
    }

    if (size == 0) {
        k4c_heap_vtable_dealloc(ctx, ptr);
        return NULL;
    }

    if (k4c_align_up_overflow(size, K4C_MEMORY_ALIGN, &size)) {
        return NULL;
    }

    k4c_heap_block *block = (k4c_heap_block *)((uint8_t *)ptr - sizeof(k4c_heap_block));
    if (block->size >= size) {
        k4c_heap_split_block(k4c_heap, block, size);
        return ptr;
    }

    k4c_heap_block *next = k4c_heap_next(block);
    if (next != NULL && next->is_free
        && (block->size + sizeof(k4c_heap_block) + next->size) >= size) {
        block->size += sizeof(k4c_heap_block) + next->size;
        k4c_doubly_linked_list_remove(k4c_heap->blocks, &next->node);
        k4c_heap_split_block(k4c_heap, block, size);
        return ptr;
    }

    void *new_ptr = k4c_heap_vtable_alloc(ctx, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);
    k4c_heap_vtable_dealloc(ctx, ptr);
    return new_ptr;
}

k4c_status k4c_heap_create(size_t capacity, k4c_heap **out) {
    K4C_ASSERT(
        capacity > sizeof(k4c_heap_block) + K4C_MEMORY_ALIGN,
        "fatal: k4c_heap_create invalid capacity"
    );
    K4C_ASSERT(out != NULL, "fatal: k4c_heap_create invalid arguments");

    *out = NULL;

    if (k4c_align_up_overflow(capacity, K4C_MEMORY_ALIGN, &capacity)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_heap *k4c_heap = NULL;
    k4c_status st = k4c_alloc(NULL, sizeof(*k4c_heap), (void **)&k4c_heap);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    st = k4c_alloc(NULL, capacity, &k4c_heap->buffer);
    if (st != K4C_STATUS_OK) {
        k4c_dealloc(NULL, k4c_heap);
        return st;
    }

    k4c_heap->capacity = capacity;
    st = k4c_doubly_linked_list_create(NULL, &k4c_heap->blocks);
    if (st != K4C_STATUS_OK) {
        k4c_dealloc(NULL, k4c_heap->buffer);
        k4c_dealloc(NULL, k4c_heap);
        return st;
    }

    k4c_heap_block *block = (k4c_heap_block *)k4c_heap->buffer;
    block->size = capacity - sizeof(k4c_heap_block);
    block->is_free = true;
    k4c_doubly_linked_list_push(k4c_heap->blocks, &block->node);

    *out = k4c_heap;
    return K4C_STATUS_OK;
}

k4c_allocator k4c_heap_allocator_view(k4c_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_heap_allocator invalid arguments");
    K4C_ASSERT(k4c_heap->buffer != NULL, "fatal: k4c_heap_allocator invalid k4c_heap");
    K4C_ASSERT(k4c_heap->blocks != NULL, "fatal: k4c_heap_allocator invalid k4c_heap");

    return (k4c_allocator){
        .ctx = k4c_heap,
        .features = K4C_ALLOCATOR_FEATURE_DEALLOC | K4C_ALLOCATOR_FEATURE_REALLOC,
        .vtable = &k4c_heap_allocator_vtable,
    };
}

size_t k4c_heap_capacity(const k4c_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_heap_capacity invalid arguments");
    K4C_ASSERT(k4c_heap->buffer != NULL, "fatal: k4c_heap_capacity invalid k4c_heap");
    K4C_ASSERT(k4c_heap->blocks != NULL, "fatal: k4c_heap_capacity invalid k4c_heap");

    return k4c_heap->capacity;
}

size_t k4c_heap_available(const k4c_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_heap_available invalid arguments");
    K4C_ASSERT(k4c_heap->buffer != NULL, "fatal: k4c_heap_available invalid k4c_heap");
    K4C_ASSERT(k4c_heap->blocks != NULL, "fatal: k4c_heap_available invalid k4c_heap");

    size_t total = 0;
    const k4c_heap_block *block = k4c_heap_head(k4c_heap);
    while (block != NULL) {
        if (block->is_free) {
            total += block->size;
        }
        block = k4c_heap_next(block);
    }

    return total;
}

void k4c_heap_destroy(k4c_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_heap_destroy invalid arguments");

    if (k4c_heap->blocks != NULL) {
        k4c_doubly_linked_list_destroy(k4c_heap->blocks);
    }

    k4c_dealloc(NULL, k4c_heap->buffer);
    k4c_dealloc(NULL, k4c_heap);
}
