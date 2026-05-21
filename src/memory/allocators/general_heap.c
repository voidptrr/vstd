#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/allocators/general_heap.h"
#include "memory/utils.h"

#define CKIT_HEAP_ALIGN (sizeof(max_align_t))

/* Header stored immediately before each heap-managed payload block. */
typedef struct ckit_heap_block {
    size_t size;
    struct ckit_heap_block *next;
    struct ckit_heap_block *prev;
    bool is_free;
} ckit_heap_block;

static ckit_heap_block *ckit_heap_head(const ckit_heap *heap) {
    return (ckit_heap_block *)heap->head;
}

static void ckit_heap_split_block(ckit_heap_block *block, size_t size) {
    size_t remaining = block->size - size;

    if (remaining <= sizeof(ckit_heap_block) + CKIT_HEAP_ALIGN) {
        return;
    }

    uint8_t *next_addr = (uint8_t *)(block + 1) + size;
    ckit_heap_block *split = (ckit_heap_block *)next_addr;
    split->size = remaining - sizeof(ckit_heap_block);
    split->is_free = true;
    split->prev = block;
    split->next = block->next;

    if (split->next != NULL) {
        split->next->prev = split;
    }

    block->size = size;
    block->next = split;
}

static void ckit_heap_coalesce(ckit_heap_block *block) {
    if (block->next != NULL && block->next->is_free) {
        ckit_heap_block *next = block->next;
        block->size += sizeof(ckit_heap_block) + next->size;
        block->next = next->next;
        if (block->next != NULL) {
            block->next->prev = block;
        }
    }

    if (block->prev != NULL && block->prev->is_free) {
        ckit_heap_block *prev = block->prev;
        prev->size += sizeof(ckit_heap_block) + block->size;
        prev->next = block->next;
        if (prev->next != NULL) {
            prev->next->prev = prev;
        }
    }
}

ckit_allocator ckit_heap_init(ckit_heap *heap, size_t capacity) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_init invalid arguments");

    capacity = ckit_align_up(capacity, CKIT_HEAP_ALIGN);
    CKIT_ASSERT(capacity > sizeof(ckit_heap_block) + CKIT_HEAP_ALIGN,
                "fatal: ckit_heap_init invalid capacity");

    heap->buffer = ckit_malloc(NULL, capacity);
    heap->capacity = capacity;
    heap->head = heap->buffer;

    ckit_heap_block *block = (ckit_heap_block *)heap->head;
    block->size = capacity - sizeof(ckit_heap_block);
    block->next = NULL;
    block->prev = NULL;
    block->is_free = true;

    ckit_allocator allocator;
    allocator.ctx = heap;
    allocator.alloc = (ckit_alloc_fn)ckit_heap_alloc;
    allocator.realloc = (ckit_realloc_fn)ckit_heap_realloc;
    allocator.dealloc = (ckit_dealloc_fn)ckit_heap_dealloc;

    return allocator;
}

void ckit_heap_free(ckit_heap *heap) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_heap_free invalid arguments");

    free(heap->buffer);
    heap->buffer = NULL;
    heap->capacity = 0;
    heap->head = NULL;
}

void *ckit_heap_alloc(ckit_heap *heap, size_t size) {
    if (heap == NULL || size == 0U || heap->head == NULL) {
        return NULL;
    }

    size = ckit_align_up(size, CKIT_HEAP_ALIGN);
    ckit_heap_block *block = ckit_heap_head(heap);
    while (block != NULL) {
        if (block->is_free && block->size >= size) {
            ckit_heap_split_block(block, size);
            block->is_free = false;
            return (void *)(block + 1);
        }
        block = block->next;
    }

    return NULL;
}

void ckit_heap_dealloc(ckit_heap *heap, void *ptr) {
    if (heap == NULL || ptr == NULL || heap->buffer == NULL) {
        return;
    }

    uintptr_t start = (uintptr_t)heap->buffer;
    uintptr_t end = start + heap->capacity;
    uintptr_t value = (uintptr_t)ptr;
    if (value <= start || value >= end) {
        return;
    }

    ckit_heap_block *block = ((ckit_heap_block *)ptr) - 1;
    block->is_free = true;
    ckit_heap_coalesce(block);
}

void *ckit_heap_realloc(ckit_heap *heap, void *ptr, size_t size) {
    if (heap == NULL) {
        return NULL;
    }

    if (ptr == NULL) {
        return ckit_heap_alloc(heap, size);
    }

    if (size == 0U) {
        ckit_heap_dealloc(heap, ptr);
        return NULL;
    }

    size = ckit_align_up(size, CKIT_HEAP_ALIGN);
    ckit_heap_block *block = ((ckit_heap_block *)ptr) - 1;
    if (block->size >= size) {
        ckit_heap_split_block(block, size);
        return ptr;
    }

    if (block->next != NULL && block->next->is_free &&
        (block->size + sizeof(ckit_heap_block) + block->next->size) >= size) {
        ckit_heap_block *next = block->next;
        block->size += sizeof(ckit_heap_block) + next->size;
        block->next = next->next;
        if (block->next != NULL) {
            block->next->prev = block;
        }
        ckit_heap_split_block(block, size);
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
    if (heap == NULL) {
        return 0U;
    }
    return heap->capacity;
}

size_t ckit_heap_available(const ckit_heap *heap) {
    size_t total = 0U;

    if (heap == NULL || heap->head == NULL) {
        return 0U;
    }

    const ckit_heap_block *block = ckit_heap_head(heap);
    while (block != NULL) {
        if (block->is_free) {
            total += block->size;
        }
        block = block->next;
    }

    return total;
}
