# memory.general_heap

## DESCRIPTION

The heap module provides an in-process free-list allocator backed by an
internal contiguous region. It supports allocation, deallocation, and reallocation
using a coalescing free-list strategy.

This API is fail-fast for required initialization/teardown preconditions.

## TYPES

### ck_heap

```c
typedef struct ck_heap ck_heap;
```

- Notes: `ck_heap` is opaque. Use the functions below to inspect or mutate
  heap state.

## FUNCTIONS

### ck_heap_init

```c
ck_heap *ck_heap_init(size_t capacity);
```

- Parameters: `capacity`
- Returns: heap pointer.

### ck_heap_allocator

```c
ck_allocator ck_heap_allocator(ck_heap *heap);
```

- Parameters: `heap`
- Returns: allocator adapter bound to `heap`.
- Notes: the returned allocator advertises `CK_ALLOCATOR_FEATURE_DEALLOC | CK_ALLOCATOR_FEATURE_REALLOC`.

### ck_heap_alloc

```c
void *ck_heap_alloc(ck_heap *heap, size_t size);
```

- Parameters: `heap`, `size`
- Returns: allocated pointer, or `NULL` when allocation cannot be satisfied.

### ck_heap_dealloc

```c
void ck_heap_dealloc(ck_heap *heap, void *ptr);
```

- Parameters: `heap`, `ptr`
- Behavior: frees `ptr` if valid.

### ck_heap_realloc

```c
void *ck_heap_realloc(ck_heap *heap, void *ptr, size_t size);
```

- Parameters: `heap`, `ptr`, `size`
- Returns: resized pointer, or `NULL` on failure.
- Notes: `ptr == NULL` behaves like allocation; `size == 0` frees `ptr` and returns `NULL`.

### ck_heap_capacity

```c
size_t ck_heap_capacity(const ck_heap *heap);
```

- Parameters: `heap`
- Returns: total managed bytes.

### ck_heap_available

```c
size_t ck_heap_available(const ck_heap *heap);
```

- Parameters: `heap`
- Returns: sum of currently free payload bytes.

### ck_heap_deinit

```c
void ck_heap_deinit(ck_heap *heap);
```

- Parameters: `heap`
- Returns: none.
- Behavior: releases the backing buffer and heap handle.
