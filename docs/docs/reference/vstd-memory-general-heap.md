# memory.general_heap

## DESCRIPTION

The heap module provides an in-process free-list allocator backed by an
internal contiguous region. It supports allocation, deallocation, and reallocation
using a coalescing free-list strategy.

This API is fail-fast for required initialization/teardown preconditions.

## TYPES

### vs_heap

```c
typedef struct vs_heap vs_heap;
```

- Notes: `vs_heap` is opaque. Use the functions below to inspect or mutate
  heap state.

## FUNCTIONS

### vs_heap_create

```c
vs_status vs_heap_create(size_t capacity, vs_heap **out);
```

- Parameters: `capacity`, `out`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Writes: heap pointer to `*out` on success.

### vs_heap_allocator

```c
vs_allocator *vs_heap_allocator(vs_heap *heap);
```

- Parameters: `heap`
- Returns: generic allocator view owned by `heap`.
- Notes: the returned allocator is owned by `heap` and advertises `VS_ALLOCATOR_FEATURE_DEALLOC | VS_ALLOCATOR_FEATURE_REALLOC`.

### vs_heap_alloc

```c
void *vs_heap_alloc(vs_heap *heap, size_t size);
```

- Parameters: `heap`, `size`
- Returns: allocated pointer, or `NULL` when allocation cannot be satisfied.

### vs_heap_dealloc

```c
void vs_heap_dealloc(vs_heap *heap, void *ptr);
```

- Parameters: `heap`, `ptr`
- Behavior: frees `ptr` if valid.

### vs_heap_realloc

```c
void *vs_heap_realloc(vs_heap *heap, void *ptr, size_t size);
```

- Parameters: `heap`, `ptr`, `size`
- Returns: resized pointer, or `NULL` on failure.
- Notes: `ptr == NULL` behaves like allocation; `size == 0` frees `ptr` and returns `NULL`.

### vs_heap_capacity

```c
size_t vs_heap_capacity(const vs_heap *heap);
```

- Parameters: `heap`
- Returns: total managed bytes.

### vs_heap_available

```c
size_t vs_heap_available(const vs_heap *heap);
```

- Parameters: `heap`
- Returns: sum of currently free payload bytes.

### vs_heap_destroy

```c
void vs_heap_destroy(vs_heap *heap);
```

- Parameters: `heap`
- Returns: none.
- Behavior: releases the backing buffer and heap handle.
