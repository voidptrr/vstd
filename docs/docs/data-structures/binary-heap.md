# datastruct.binary_heap

## DESCRIPTION

The binary_heap module provides a generic binary heap backed by contiguous vector storage.
Heap ordering is defined by a user-supplied comparator function.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ckit_binary_heap_init

```c
ckit_binary_heap *ckit_binary_heap_init(size_t elem_size,
                                        ckit_heap_cmp_fn cmp,
                                        ckit_allocator *allocator);
```

- Parameters: `elem_size`, `cmp`, `allocator`
- Returns: opaque binary-heap handle.
- Notes: when `allocator` is `NULL`, binary heap storage uses the C library heap through `ckit_malloc`.

### ckit_binary_heap_push

```c
void ckit_binary_heap_push(ckit_binary_heap *heap, const void *element);
```

- Parameters: `heap`, `element`
- Returns: none.

### ckit_binary_heap_pop

```c
void *ckit_binary_heap_pop(ckit_binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to removed top element in heap-managed storage, or `NULL` when empty.

### ckit_binary_heap_peek

```c
const void *ckit_binary_heap_peek(const ckit_binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to top element in heap-managed storage, or `NULL` when empty.

### ckit_binary_heap_free

```c
void ckit_binary_heap_free(ckit_binary_heap *heap);
```

- Parameters: `heap`
- Returns: none.
- Notes: releases heap storage and the opaque handle. Do not use `heap` after this call.

### ckit_binary_heap_size

```c
size_t ckit_binary_heap_size(const ckit_binary_heap *heap);
```

- Parameters: `heap`
- Returns: current element count.
- Notes: returns `0` when `heap` is `NULL`.

### ckit_binary_heap_is_empty

```c
bool ckit_binary_heap_is_empty(const ckit_binary_heap *heap);
```

- Parameters: `heap`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `heap` is `NULL`.
