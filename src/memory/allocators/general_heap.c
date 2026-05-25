#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/doubly_linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/allocators/general_heap.h"
#include "ckit/memory/utils.h"

/* Header stored immediately before each heap-managed payload block. */
typedef struct ckit_heap_block {
    size_t size;
    ckit_doubly_linked_list_node node;
    bool is_free;
} ckit_heap_block;

struct ckit_heap {
    void *buffer;
    size_t capacity;
    ckit_doubly_linked_list *blocks;
};

static ckit_heap_block *ckit_heap_head(const ckit_heap *heap) {
    ckit_doubly_linked_list_node *head = ckit_doubly_linked_list_head(heap->blocks);
    if (head == NULL) {
        return NULL;
    }

    return CKIT_CONTAINER_OF(head, ckit_heap_block, node);
}

static ckit_heap_block *ckit_heap_next(const ckit_heap_block *block) {
    if (block->node.next == NULL) {
        return NULL;
    }

    return CKIT_CONTAINER_OF(block->node.next, ckit_heap_block, node);
}

static ckit_heap_block *ckit_heap_prev(const ckit_heap_block *block) {
    if (block->node.prev == NULL) {
        return NULL;
    }

    return CKIT_CONTAINER_OF(block->node.prev, ckit_heap_block, node);
}

static void ckit_heap_split_block(ckit_heap *heap, ckit_heap_block *block, size_t size) {
    size_t remaining = block->size - size;

    if (remaining <= sizeof(ckit_heap_block) + CKIT_MEMORY_ALIGN) {
        return;
    }

    ckit_heap_block *split = (ckit_heap_block *)((uint8_t *)block + sizeof(*block) + size);
    split->size = remaining - sizeof(ckit_heap_block);
    split->is_free = true;

    block->size = size;
    ckit_doubly_linked_list_insert_after(heap->blocks, &block->node, &split->node);
}

static ckit_heap_block *ckit_heap_coalesce(ckit_heap *heap, ckit_heap_block *block) {
    ckit_heap_block *next = ckit_heap_next(block);
    if (next != NULL && next->is_free) {
        block->size += sizeof(ckit_heap_block) + next->size;
        ckit_doubly_linked_list_remove(heap->blocks, &next->node);
    }

    ckit_heap_block *prev = ckit_heap_prev(block);
    if (prev != NULL && prev->is_free) {
        prev->size += sizeof(ckit_heap_block) + block->size;
        ckit_doubly_linked_list_remove(heap->blocks, &block->node);
        block = prev;
    }

    return block;
}

ckit_heap *ckit_heap_init(size_t capacity) {
    ckit_heap *heap;

    capacity = ckit_align_up(capacity, CKIT_MEMORY_ALIGN);
    CKIT_ASSERT(capacity > sizeof(ckit_heap_block) + CKIT_MEMORY_ALIGN,
                "fatal: ckit_heap_init invalid capacity");

    heap = ckit_malloc(NULL, sizeof(*heap));
    heap->buffer = ckit_malloc(NULL, capacity);
    heap->capacity = capacity;
    heap->blocks = ckit_doubly_linked_list_init(NULL);

    ckit_heap_block *block = (ckit_heap_block *)heap->buffer;
    block->size = capacity - sizeof(ckit_heap_block);
    block->is_free = true;
    ckit_doubly_linked_list_push(heap->blocks, &block->node);

    return heap;
}

ckit_allocator ckit_heap_allocator(ckit_heap *heap) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_allocator invalid arguments");
    CKIT_ASSERT(heap->buffer != NULL, "fatal: ckit_heap_allocator invalid heap");
    CKIT_ASSERT(heap->blocks != NULL, "fatal: ckit_heap_allocator invalid heap");

    ckit_allocator allocator;
    allocator.ctx = heap;
    allocator.features = CKIT_ALLOCATOR_FEATURE_DEALLOC | CKIT_ALLOCATOR_FEATURE_REALLOC;
    allocator.alloc = (ckit_alloc_fn)ckit_heap_alloc;
    allocator.realloc = (ckit_realloc_fn)ckit_heap_realloc;
    allocator.dealloc = (ckit_dealloc_fn)ckit_heap_dealloc;

    return allocator;
}

void *ckit_heap_alloc(ckit_heap *heap, size_t size) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_alloc invalid arguments");
    CKIT_ASSERT(heap->buffer != NULL, "fatal: ckit_heap_alloc invalid heap");
    CKIT_ASSERT(heap->blocks != NULL, "fatal: ckit_heap_alloc invalid heap");
    CKIT_ASSERT(size > 0, "fatal: ckit_heap_alloc invalid arguments");

    size = ckit_align_up(size, CKIT_MEMORY_ALIGN);
    ckit_heap_block *block = ckit_heap_head(heap);
    while (block != NULL) {
        if (block->is_free && block->size >= size) {
            ckit_heap_split_block(heap, block, size);
            block->is_free = false;
            return (uint8_t *)block + sizeof(*block);
        }
        block = ckit_heap_next(block);
    }

    return NULL;
}

void ckit_heap_dealloc(ckit_heap *heap, void *ptr) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_dealloc invalid arguments");
    CKIT_ASSERT(ptr != NULL, "fatal: ckit_heap_dealloc invalid arguments");
    CKIT_ASSERT(heap->buffer != NULL, "fatal: ckit_heap_dealloc invalid heap");
    CKIT_ASSERT(heap->blocks != NULL, "fatal: ckit_heap_dealloc invalid heap");

    uintptr_t start = (uintptr_t)heap->buffer;
    uintptr_t end = start + heap->capacity;
    uintptr_t value = (uintptr_t)ptr;
    if (value <= start || value >= end) {
        return;
    }

    ckit_heap_block *block = (ckit_heap_block *)((uint8_t *)ptr - sizeof(ckit_heap_block));
    block->is_free = true;
    ckit_heap_coalesce(heap, block);
}

void *ckit_heap_realloc(ckit_heap *heap, void *ptr, size_t size) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_realloc invalid arguments");
    CKIT_ASSERT(heap->buffer != NULL, "fatal: ckit_heap_realloc invalid heap");
    CKIT_ASSERT(heap->blocks != NULL, "fatal: ckit_heap_realloc invalid heap");

    if (ptr == NULL) {
        return ckit_heap_alloc(heap, size);
    }

    if (size == 0) {
        ckit_heap_dealloc(heap, ptr);
        return NULL;
    }

    size = ckit_align_up(size, CKIT_MEMORY_ALIGN);
    ckit_heap_block *block = (ckit_heap_block *)((uint8_t *)ptr - sizeof(ckit_heap_block));
    if (block->size >= size) {
        ckit_heap_split_block(heap, block, size);
        return ptr;
    }

    ckit_heap_block *next = ckit_heap_next(block);
    if (next != NULL && next->is_free &&
        (block->size + sizeof(ckit_heap_block) + next->size) >= size) {
        block->size += sizeof(ckit_heap_block) + next->size;
        ckit_doubly_linked_list_remove(heap->blocks, &next->node);
        ckit_heap_split_block(heap, block, size);
        return ptr;
    }

    void *new_ptr = ckit_heap_alloc(heap, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);
    ckit_heap_dealloc(heap, ptr);
    return new_ptr;
}

size_t ckit_heap_capacity(const ckit_heap *heap) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_capacity invalid arguments");
    CKIT_ASSERT(heap->buffer != NULL, "fatal: ckit_heap_capacity invalid heap");
    CKIT_ASSERT(heap->blocks != NULL, "fatal: ckit_heap_capacity invalid heap");

    return heap->capacity;
}

size_t ckit_heap_available(const ckit_heap *heap) {
    size_t total = 0;

    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_available invalid arguments");
    CKIT_ASSERT(heap->buffer != NULL, "fatal: ckit_heap_available invalid heap");
    CKIT_ASSERT(heap->blocks != NULL, "fatal: ckit_heap_available invalid heap");

    const ckit_heap_block *block = ckit_heap_head(heap);
    while (block != NULL) {
        if (block->is_free) {
            total += block->size;
        }
        block = ckit_heap_next(block);
    }

    return total;
}

void ckit_heap_deinit(ckit_heap *heap) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_deinit invalid arguments");

    if (heap->blocks != NULL) {
        ckit_doubly_linked_list_deinit(heap->blocks);
    }

    free(heap->buffer);
    free(heap);
}
