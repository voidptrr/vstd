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

#include "vstd/datastruct/doubly_linked_list.h"
#include "vstd/memory/allocators/allocator.h"
#include "vstd/memory/allocators/general_heap.h"
#include "vstd/memory/utils.h"
#include "vstd/panic.h"

/* Header stored immediately before each heap-managed payload block. */
typedef struct vs_heap_block {
    size_t size;
    vs_doubly_linked_list_node node;
    bool is_free;
} vs_heap_block;

struct vs_heap {
    void *buffer;
    size_t capacity;
    vs_doubly_linked_list *blocks;
};

static vs_heap_block *vs_heap_head(const vs_heap *heap) {
    vs_doubly_linked_list_node *head = vs_doubly_linked_list_head(heap->blocks);
    if (head == NULL) {
        return NULL;
    }

    return VS_CONTAINER_OF(head, vs_heap_block, node);
}

static vs_heap_block *vs_heap_next(const vs_heap_block *block) {
    if (block->node.next == NULL) {
        return NULL;
    }

    return VS_CONTAINER_OF(block->node.next, vs_heap_block, node);
}

static vs_heap_block *vs_heap_prev(const vs_heap_block *block) {
    if (block->node.prev == NULL) {
        return NULL;
    }

    return VS_CONTAINER_OF(block->node.prev, vs_heap_block, node);
}

static void vs_heap_split_block(vs_heap *heap, vs_heap_block *block, size_t size) {
    size_t remaining = block->size - size;

    if (remaining <= sizeof(vs_heap_block) + VS_MEMORY_ALIGN) {
        return;
    }

    vs_heap_block *split = (vs_heap_block *)((uint8_t *)block + sizeof(vs_heap_block) + size);
    split->size = remaining - sizeof(vs_heap_block);
    split->is_free = true;

    block->size = size;
    vs_doubly_linked_list_insert_after(heap->blocks, &block->node, &split->node);
}

static vs_heap_block *vs_heap_coalesce(vs_heap *heap, vs_heap_block *block) {
    vs_heap_block *next = vs_heap_next(block);
    if (next != NULL && next->is_free) {
        block->size += sizeof(vs_heap_block) + next->size;
        vs_doubly_linked_list_remove(heap->blocks, &next->node);
    }

    vs_heap_block *prev = vs_heap_prev(block);
    if (prev != NULL && prev->is_free) {
        prev->size += sizeof(vs_heap_block) + block->size;
        vs_doubly_linked_list_remove(heap->blocks, &block->node);
        block = prev;
    }

    return block;
}

vs_heap *vs_heap_create(size_t capacity) {
    vs_heap *heap;

    capacity = vs_align_up(capacity, VS_MEMORY_ALIGN);
    VS_ASSERT(
        capacity > sizeof(vs_heap_block) + VS_MEMORY_ALIGN,
        "fatal: vs_heap_create invalid capacity"
    );

    heap = vs_malloc(NULL, sizeof(vs_heap));
    heap->buffer = vs_malloc(NULL, capacity);
    heap->capacity = capacity;
    heap->blocks = vs_doubly_linked_list_create(NULL);

    vs_heap_block *block = (vs_heap_block *)heap->buffer;
    block->size = capacity - sizeof(vs_heap_block);
    block->is_free = true;
    vs_doubly_linked_list_push(heap->blocks, &block->node);

    return heap;
}

vs_allocator vs_heap_adapter(vs_heap *heap) {
    VS_ASSERT(heap != NULL, "fatal: vs_heap_adapter invalid arguments");
    VS_ASSERT(heap->buffer != NULL, "fatal: vs_heap_adapter invalid heap");
    VS_ASSERT(heap->blocks != NULL, "fatal: vs_heap_adapter invalid heap");

    vs_allocator allocator;
    allocator.ctx = heap;
    allocator.features = VS_ALLOCATOR_FEATURE_DEALLOC | VS_ALLOCATOR_FEATURE_REALLOC;
    allocator.alloc = (vs_alloc_fn)vs_heap_alloc;
    allocator.realloc = (vs_realloc_fn)vs_heap_realloc;
    allocator.dealloc = (vs_dealloc_fn)vs_heap_dealloc;

    return allocator;
}

void *vs_heap_alloc(vs_heap *heap, size_t size) {
    VS_ASSERT(heap != NULL, "fatal: vs_heap_alloc invalid arguments");
    VS_ASSERT(heap->buffer != NULL, "fatal: vs_heap_alloc invalid heap");
    VS_ASSERT(heap->blocks != NULL, "fatal: vs_heap_alloc invalid heap");
    VS_ASSERT(size > 0, "fatal: vs_heap_alloc invalid arguments");

    size = vs_align_up(size, VS_MEMORY_ALIGN);
    vs_heap_block *block = vs_heap_head(heap);
    while (block != NULL) {
        if (block->is_free && block->size >= size) {
            vs_heap_split_block(heap, block, size);
            block->is_free = false;
            return (uint8_t *)block + sizeof(vs_heap_block);
        }
        block = vs_heap_next(block);
    }

    return NULL;
}

void vs_heap_dealloc(vs_heap *heap, void *ptr) {
    VS_ASSERT(heap != NULL, "fatal: vs_heap_dealloc invalid arguments");
    VS_ASSERT(ptr != NULL, "fatal: vs_heap_dealloc invalid arguments");
    VS_ASSERT(heap->buffer != NULL, "fatal: vs_heap_dealloc invalid heap");
    VS_ASSERT(heap->blocks != NULL, "fatal: vs_heap_dealloc invalid heap");

    uintptr_t start = (uintptr_t)heap->buffer;
    uintptr_t end = start + heap->capacity;
    uintptr_t value = (uintptr_t)ptr;
    if (value <= start || value >= end) {
        return;
    }

    vs_heap_block *block = (vs_heap_block *)((uint8_t *)ptr - sizeof(vs_heap_block));
    block->is_free = true;
    vs_heap_coalesce(heap, block);
}

void *vs_heap_realloc(vs_heap *heap, void *ptr, size_t size) {
    VS_ASSERT(heap != NULL, "fatal: vs_heap_realloc invalid arguments");
    VS_ASSERT(heap->buffer != NULL, "fatal: vs_heap_realloc invalid heap");
    VS_ASSERT(heap->blocks != NULL, "fatal: vs_heap_realloc invalid heap");

    if (ptr == NULL) {
        return vs_heap_alloc(heap, size);
    }

    if (size == 0) {
        vs_heap_dealloc(heap, ptr);
        return NULL;
    }

    size = vs_align_up(size, VS_MEMORY_ALIGN);
    vs_heap_block *block = (vs_heap_block *)((uint8_t *)ptr - sizeof(vs_heap_block));
    if (block->size >= size) {
        vs_heap_split_block(heap, block, size);
        return ptr;
    }

    vs_heap_block *next = vs_heap_next(block);
    if (next != NULL && next->is_free
        && (block->size + sizeof(vs_heap_block) + next->size) >= size) {
        block->size += sizeof(vs_heap_block) + next->size;
        vs_doubly_linked_list_remove(heap->blocks, &next->node);
        vs_heap_split_block(heap, block, size);
        return ptr;
    }

    void *new_ptr = vs_heap_alloc(heap, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);
    vs_heap_dealloc(heap, ptr);
    return new_ptr;
}

size_t vs_heap_capacity(const vs_heap *heap) {
    VS_ASSERT(heap != NULL, "fatal: vs_heap_capacity invalid arguments");
    VS_ASSERT(heap->buffer != NULL, "fatal: vs_heap_capacity invalid heap");
    VS_ASSERT(heap->blocks != NULL, "fatal: vs_heap_capacity invalid heap");

    return heap->capacity;
}

size_t vs_heap_available(const vs_heap *heap) {
    size_t total = 0;

    VS_ASSERT(heap != NULL, "fatal: vs_heap_available invalid arguments");
    VS_ASSERT(heap->buffer != NULL, "fatal: vs_heap_available invalid heap");
    VS_ASSERT(heap->blocks != NULL, "fatal: vs_heap_available invalid heap");

    const vs_heap_block *block = vs_heap_head(heap);
    while (block != NULL) {
        if (block->is_free) {
            total += block->size;
        }
        block = vs_heap_next(block);
    }

    return total;
}

void vs_heap_destroy(vs_heap *heap) {
    VS_ASSERT(heap != NULL, "fatal: vs_heap_destroy invalid arguments");

    if (heap->blocks != NULL) {
        vs_doubly_linked_list_destroy(heap->blocks);
    }

    vs_dealloc(NULL, heap->buffer);
    vs_dealloc(NULL, heap);
}
