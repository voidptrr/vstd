# memory.general_heap

## DESCRIPTION

The heap module provides an in-process free-list allocator backed by an
internal contiguous region. It supports allocation, deallocation, and reallocation
using a coalescing free-list strategy.

This API is fail-fast for required initialization/teardown preconditions.

## TYPES

### heap

```c
typedef struct heap heap;
```

- Notes: `heap` is opaque. Use the functions below to inspect or mutate
  heap state.

## FUNCTIONS

### heap_create

```c
status heap_create(size_t capacity, heap **out);
```

- Parameters: `capacity`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: heap pointer to `*out` on success.

### heap_allocator

```c
allocator *heap_allocator(heap *heap);
```

- Parameters: `heap`
- Returns: generic allocator view owned by `heap`.
- Notes: the returned allocator is owned by `heap` and advertises `ALLOCATOR_FEATURE_DEALLOC | ALLOCATOR_FEATURE_REALLOC`.

### heap_alloc

```c
void *heap_alloc(heap *heap, size_t size);
```

- Parameters: `heap`, `size`
- Returns: allocated pointer, or `NULL` when allocation cannot be satisfied.

### heap_dealloc

```c
void heap_dealloc(heap *heap, void *ptr);
```

- Parameters: `heap`, `ptr`
- Behavior: frees `ptr` if valid.

### heap_realloc

```c
void *heap_realloc(heap *heap, void *ptr, size_t size);
```

- Parameters: `heap`, `ptr`, `size`
- Returns: resized pointer, or `NULL` on failure.
- Notes: `ptr == NULL` behaves like allocation; `size == 0` frees `ptr` and returns `NULL`.

### heap_capacity

```c
size_t heap_capacity(const heap *heap);
```

- Parameters: `heap`
- Returns: total managed bytes.

### heap_available

```c
size_t heap_available(const heap *heap);
```

- Parameters: `heap`
- Returns: sum of currently free payload bytes.

### heap_destroy

```c
void heap_destroy(heap *heap);
```

- Parameters: `heap`
- Returns: none.
- Behavior: releases the backing buffer and heap handle.
