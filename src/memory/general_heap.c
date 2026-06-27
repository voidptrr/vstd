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

#include "vstd/assert.h"
#include "vstd/ds/doubly_linked_list.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/general_heap.h"
#include "vstd/memory/utils.h"

/* Header stored immediately before each heap-managed payload block. */
typedef struct heap_block {
    size_t size;
    doubly_linked_list_node node;
    bool is_free;
} heap_block;

struct heap {
    allocator allocator;
    void *buffer;
    size_t capacity;
    doubly_linked_list *blocks;
};

static heap_block *heap_head(const heap *heap) {
    doubly_linked_list_node *head = doubly_linked_list_head(heap->blocks);
    if (head == NULL) {
        return NULL;
    }

    return CONTAINER_OF(head, heap_block, node);
}

static heap_block *heap_next(const heap_block *block) {
    if (block->node.next == NULL) {
        return NULL;
    }

    return CONTAINER_OF(block->node.next, heap_block, node);
}

static heap_block *heap_prev(const heap_block *block) {
    if (block->node.prev == NULL) {
        return NULL;
    }

    return CONTAINER_OF(block->node.prev, heap_block, node);
}

static void heap_split_block(heap *heap, heap_block *block, size_t size) {
    size_t remaining = block->size - size;

    if (remaining <= sizeof(heap_block) + MEMORY_ALIGN) {
        return;
    }

    heap_block *split = (heap_block *)((uint8_t *)block + sizeof(heap_block) + size);
    split->size = remaining - sizeof(heap_block);
    split->is_free = true;

    block->size = size;
    doubly_linked_list_insert_after(heap->blocks, &block->node, &split->node);
}

static heap_block *heap_coalesce(heap *heap, heap_block *block) {
    heap_block *next = heap_next(block);
    if (next != NULL && next->is_free) {
        block->size += sizeof(heap_block) + next->size;
        doubly_linked_list_remove(heap->blocks, &next->node);
    }

    heap_block *prev = heap_prev(block);
    if (prev != NULL && prev->is_free) {
        prev->size += sizeof(heap_block) + block->size;
        doubly_linked_list_remove(heap->blocks, &block->node);
        block = prev;
    }

    return block;
}

void *heap_alloc(heap *heap, size_t size) {
    ASSERT(heap != NULL, "fatal: heap_alloc invalid arguments");
    ASSERT(heap->buffer != NULL, "fatal: heap_alloc invalid heap");
    ASSERT(heap->blocks != NULL, "fatal: heap_alloc invalid heap");
    ASSERT(size > 0, "fatal: heap_alloc invalid arguments");

    if (align_up_overflow(size, MEMORY_ALIGN, &size)) {
        return NULL;
    }

    heap_block *block = heap_head(heap);
    while (block != NULL) {
        if (block->is_free && block->size >= size) {
            heap_split_block(heap, block, size);
            block->is_free = false;
            return (uint8_t *)block + sizeof(heap_block);
        }
        block = heap_next(block);
    }

    return NULL;
}

void heap_dealloc(heap *heap, void *ptr) {
    ASSERT(heap != NULL, "fatal: heap_dealloc invalid arguments");
    ASSERT(ptr != NULL, "fatal: heap_dealloc invalid arguments");
    ASSERT(heap->buffer != NULL, "fatal: heap_dealloc invalid heap");
    ASSERT(heap->blocks != NULL, "fatal: heap_dealloc invalid heap");

    uintptr_t start = (uintptr_t)heap->buffer;
    uintptr_t end = start + heap->capacity;
    uintptr_t value = (uintptr_t)ptr;
    if (value <= start || value >= end) {
        return;
    }

    heap_block *block = (heap_block *)((uint8_t *)ptr - sizeof(heap_block));
    block->is_free = true;
    heap_coalesce(heap, block);
}

void *heap_realloc(heap *heap, void *ptr, size_t size) {
    ASSERT(heap != NULL, "fatal: heap_realloc invalid arguments");
    ASSERT(heap->buffer != NULL, "fatal: heap_realloc invalid heap");
    ASSERT(heap->blocks != NULL, "fatal: heap_realloc invalid heap");

    if (ptr == NULL) {
        return heap_alloc(heap, size);
    }

    if (size == 0) {
        heap_dealloc(heap, ptr);
        return NULL;
    }

    if (align_up_overflow(size, MEMORY_ALIGN, &size)) {
        return NULL;
    }

    heap_block *block = (heap_block *)((uint8_t *)ptr - sizeof(heap_block));
    if (block->size >= size) {
        heap_split_block(heap, block, size);
        return ptr;
    }

    heap_block *next = heap_next(block);
    if (next != NULL && next->is_free && (block->size + sizeof(heap_block) + next->size) >= size) {
        block->size += sizeof(heap_block) + next->size;
        doubly_linked_list_remove(heap->blocks, &next->node);
        heap_split_block(heap, block, size);
        return ptr;
    }

    void *new_ptr = heap_alloc(heap, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);
    heap_dealloc(heap, ptr);
    return new_ptr;
}

static void *heap_alloc_callback(void *ctx, size_t size) {
    return heap_alloc(ctx, size);
}

static void heap_dealloc_callback(void *ctx, void *ptr) {
    heap_dealloc(ctx, ptr);
}

static void *heap_realloc_callback(void *ctx, void *ptr, size_t size) {
    return heap_realloc(ctx, ptr, size);
}

status heap_create(size_t capacity, heap **out) {
    if (align_up_overflow(capacity, MEMORY_ALIGN, &capacity)) {
        return STATUS_OVERFLOW;
    }

    ASSERT(capacity > sizeof(heap_block) + MEMORY_ALIGN, "fatal: heap_create invalid capacity");
    ASSERT(out != NULL, "fatal: heap_create invalid arguments");

    *out = NULL;

    heap *heap = NULL;
    status st = alloc(NULL, sizeof(*heap), (void **)&heap);
    if (st != STATUS_OK) {
        return st;
    }

    st = alloc(NULL, capacity, &heap->buffer);
    if (st != STATUS_OK) {
        dealloc(NULL, heap);
        return st;
    }

    heap->capacity = capacity;
    st = doubly_linked_list_create(NULL, &heap->blocks);
    if (st != STATUS_OK) {
        dealloc(NULL, heap->buffer);
        dealloc(NULL, heap);
        return st;
    }

    heap->allocator = (allocator){
        .ctx = heap,
        .features = ALLOCATOR_FEATURE_DEALLOC | ALLOCATOR_FEATURE_REALLOC,
        .alloc = heap_alloc_callback,
        .realloc = heap_realloc_callback,
        .dealloc = heap_dealloc_callback,
    };

    heap_block *block = (heap_block *)heap->buffer;
    block->size = capacity - sizeof(heap_block);
    block->is_free = true;
    doubly_linked_list_push(heap->blocks, &block->node);

    *out = heap;
    return STATUS_OK;
}

allocator *heap_allocator(heap *heap) {
    ASSERT(heap != NULL, "fatal: heap_allocator invalid arguments");
    ASSERT(heap->buffer != NULL, "fatal: heap_allocator invalid heap");
    ASSERT(heap->blocks != NULL, "fatal: heap_allocator invalid heap");

    return &heap->allocator;
}

size_t heap_capacity(const heap *heap) {
    ASSERT(heap != NULL, "fatal: heap_capacity invalid arguments");
    ASSERT(heap->buffer != NULL, "fatal: heap_capacity invalid heap");
    ASSERT(heap->blocks != NULL, "fatal: heap_capacity invalid heap");

    return heap->capacity;
}

size_t heap_available(const heap *heap) {
    size_t total = 0;

    ASSERT(heap != NULL, "fatal: heap_available invalid arguments");
    ASSERT(heap->buffer != NULL, "fatal: heap_available invalid heap");
    ASSERT(heap->blocks != NULL, "fatal: heap_available invalid heap");

    const heap_block *block = heap_head(heap);
    while (block != NULL) {
        if (block->is_free) {
            total += block->size;
        }
        block = heap_next(block);
    }

    return total;
}

void heap_destroy(heap *heap) {
    ASSERT(heap != NULL, "fatal: heap_destroy invalid arguments");

    if (heap->blocks != NULL) {
        doubly_linked_list_destroy(heap->blocks);
    }

    dealloc(NULL, heap->buffer);
    dealloc(NULL, heap);
}
