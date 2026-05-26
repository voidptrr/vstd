#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/doubly_linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/allocators/general_heap.h"
#include "memory/utils.h"

/* Header stored immediately before each heap-managed payload block. */
typedef struct ck_heap_block {
    size_t size;
    ck_doubly_linked_list_node node;
    bool is_free;
} ck_heap_block;

struct ck_heap {
    void *buffer;
    size_t capacity;
    ck_doubly_linked_list *blocks;
};

static ck_heap_block *ck_heap_head(const ck_heap *heap) {
    ck_doubly_linked_list_node *head = ck_doubly_linked_list_head(heap->blocks);
    if (head == NULL) {
        return NULL;
    }

    return CK_CONTAINER_OF(head, ck_heap_block, node);
}

static ck_heap_block *ck_heap_next(const ck_heap_block *block) {
    if (block->node.next == NULL) {
        return NULL;
    }

    return CK_CONTAINER_OF(block->node.next, ck_heap_block, node);
}

static ck_heap_block *ck_heap_prev(const ck_heap_block *block) {
    if (block->node.prev == NULL) {
        return NULL;
    }

    return CK_CONTAINER_OF(block->node.prev, ck_heap_block, node);
}

static void ck_heap_split_block(ck_heap *heap, ck_heap_block *block, size_t size) {
    size_t remaining = block->size - size;

    if (remaining <= sizeof(ck_heap_block) + CK_MEMORY_ALIGN) {
        return;
    }

    ck_heap_block *split = (ck_heap_block *)((uint8_t *)block + sizeof(ck_heap_block) + size);
    split->size = remaining - sizeof(ck_heap_block);
    split->is_free = true;

    block->size = size;
    ck_doubly_linked_list_insert_after(heap->blocks, &block->node, &split->node);
}

static ck_heap_block *ck_heap_coalesce(ck_heap *heap, ck_heap_block *block) {
    ck_heap_block *next = ck_heap_next(block);
    if (next != NULL && next->is_free) {
        block->size += sizeof(ck_heap_block) + next->size;
        ck_doubly_linked_list_remove(heap->blocks, &next->node);
    }

    ck_heap_block *prev = ck_heap_prev(block);
    if (prev != NULL && prev->is_free) {
        prev->size += sizeof(ck_heap_block) + block->size;
        ck_doubly_linked_list_remove(heap->blocks, &block->node);
        block = prev;
    }

    return block;
}

ck_heap *ck_heap_init(size_t capacity) {
    ck_heap *heap;

    capacity = ck_align_up(capacity, CK_MEMORY_ALIGN);
    CK_ASSERT(capacity > sizeof(ck_heap_block) + CK_MEMORY_ALIGN,
              "fatal: ck_heap_init invalid capacity");

    heap = ck_malloc(NULL, sizeof(ck_heap));
    heap->buffer = ck_malloc(NULL, capacity);
    heap->capacity = capacity;
    heap->blocks = ck_doubly_linked_list_init(NULL);

    ck_heap_block *block = (ck_heap_block *)heap->buffer;
    block->size = capacity - sizeof(ck_heap_block);
    block->is_free = true;
    ck_doubly_linked_list_push(heap->blocks, &block->node);

    return heap;
}

ck_allocator ck_heap_allocator(ck_heap *heap) {
    CK_ASSERT(heap != NULL, "fatal: ck_heap_allocator invalid arguments");
    CK_ASSERT(heap->buffer != NULL, "fatal: ck_heap_allocator invalid heap");
    CK_ASSERT(heap->blocks != NULL, "fatal: ck_heap_allocator invalid heap");

    ck_allocator allocator;
    allocator.ctx = heap;
    allocator.features = CK_ALLOCATOR_FEATURE_DEALLOC | CK_ALLOCATOR_FEATURE_REALLOC;
    allocator.alloc = (ck_alloc_fn)ck_heap_alloc;
    allocator.realloc = (ck_realloc_fn)ck_heap_realloc;
    allocator.dealloc = (ck_dealloc_fn)ck_heap_dealloc;

    return allocator;
}

void *ck_heap_alloc(ck_heap *heap, size_t size) {
    CK_ASSERT(heap != NULL, "fatal: ck_heap_alloc invalid arguments");
    CK_ASSERT(heap->buffer != NULL, "fatal: ck_heap_alloc invalid heap");
    CK_ASSERT(heap->blocks != NULL, "fatal: ck_heap_alloc invalid heap");
    CK_ASSERT(size > 0, "fatal: ck_heap_alloc invalid arguments");

    size = ck_align_up(size, CK_MEMORY_ALIGN);
    ck_heap_block *block = ck_heap_head(heap);
    while (block != NULL) {
        if (block->is_free && block->size >= size) {
            ck_heap_split_block(heap, block, size);
            block->is_free = false;
            return (uint8_t *)block + sizeof(ck_heap_block);
        }
        block = ck_heap_next(block);
    }

    return NULL;
}

void ck_heap_dealloc(ck_heap *heap, void *ptr) {
    CK_ASSERT(heap != NULL, "fatal: ck_heap_dealloc invalid arguments");
    CK_ASSERT(ptr != NULL, "fatal: ck_heap_dealloc invalid arguments");
    CK_ASSERT(heap->buffer != NULL, "fatal: ck_heap_dealloc invalid heap");
    CK_ASSERT(heap->blocks != NULL, "fatal: ck_heap_dealloc invalid heap");

    uintptr_t start = (uintptr_t)heap->buffer;
    uintptr_t end = start + heap->capacity;
    uintptr_t value = (uintptr_t)ptr;
    if (value <= start || value >= end) {
        return;
    }

    ck_heap_block *block = (ck_heap_block *)((uint8_t *)ptr - sizeof(ck_heap_block));
    block->is_free = true;
    ck_heap_coalesce(heap, block);
}

void *ck_heap_realloc(ck_heap *heap, void *ptr, size_t size) {
    CK_ASSERT(heap != NULL, "fatal: ck_heap_realloc invalid arguments");
    CK_ASSERT(heap->buffer != NULL, "fatal: ck_heap_realloc invalid heap");
    CK_ASSERT(heap->blocks != NULL, "fatal: ck_heap_realloc invalid heap");

    if (ptr == NULL) {
        return ck_heap_alloc(heap, size);
    }

    if (size == 0) {
        ck_heap_dealloc(heap, ptr);
        return NULL;
    }

    size = ck_align_up(size, CK_MEMORY_ALIGN);
    ck_heap_block *block = (ck_heap_block *)((uint8_t *)ptr - sizeof(ck_heap_block));
    if (block->size >= size) {
        ck_heap_split_block(heap, block, size);
        return ptr;
    }

    ck_heap_block *next = ck_heap_next(block);
    if (next != NULL && next->is_free &&
        (block->size + sizeof(ck_heap_block) + next->size) >= size) {
        block->size += sizeof(ck_heap_block) + next->size;
        ck_doubly_linked_list_remove(heap->blocks, &next->node);
        ck_heap_split_block(heap, block, size);
        return ptr;
    }

    void *new_ptr = ck_heap_alloc(heap, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);
    ck_heap_dealloc(heap, ptr);
    return new_ptr;
}

size_t ck_heap_capacity(const ck_heap *heap) {
    CK_ASSERT(heap != NULL, "fatal: ck_heap_capacity invalid arguments");
    CK_ASSERT(heap->buffer != NULL, "fatal: ck_heap_capacity invalid heap");
    CK_ASSERT(heap->blocks != NULL, "fatal: ck_heap_capacity invalid heap");

    return heap->capacity;
}

size_t ck_heap_available(const ck_heap *heap) {
    size_t total = 0;

    CK_ASSERT(heap != NULL, "fatal: ck_heap_available invalid arguments");
    CK_ASSERT(heap->buffer != NULL, "fatal: ck_heap_available invalid heap");
    CK_ASSERT(heap->blocks != NULL, "fatal: ck_heap_available invalid heap");

    const ck_heap_block *block = ck_heap_head(heap);
    while (block != NULL) {
        if (block->is_free) {
            total += block->size;
        }
        block = ck_heap_next(block);
    }

    return total;
}

void ck_heap_deinit(ck_heap *heap) {
    CK_ASSERT(heap != NULL, "fatal: ck_heap_deinit invalid arguments");

    if (heap->blocks != NULL) {
        ck_doubly_linked_list_deinit(heap->blocks);
    }

    ck_dealloc(NULL, heap->buffer);
    ck_dealloc(NULL, heap);
}
