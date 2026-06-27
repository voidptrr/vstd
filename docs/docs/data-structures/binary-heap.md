# datastruct.binary_heap

## DESCRIPTION

The binary_heap module provides a generic binary heap backed by contiguous vector storage.
Heap ordering is defined by a caller callback. When the callback is `NULL`,
ordering is byte-wise with `memcmp` over stored element bytes.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_binary_heap_iterator

```c
typedef struct vs_binary_heap_iterator vs_binary_heap_iterator;
```

Typed cursor for walking heap backing storage.

## FUNCTIONS

### vs_binary_heap_create

```c
vs_status vs_binary_heap_create(size_t elem_size,
                                vs_binary_heap_cmp_fn cmp,
                                vs_allocator *allocator,
                                vs_binary_heap **out);
```

- Parameters: `elem_size`, `cmp`, `allocator`, `out`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Writes: opaque binary-heap handle to `*out` on success.
- Notes: the binary heap stores `allocator` and reuses it for growth and
  destroy. When `allocator` is `NULL`, binary heap storage uses the C library
  heap through `vs_alloc`/`vs_resize`. When `cmp` is `NULL`, element ordering
  uses byte comparison.
- Example:

```c
static int cmp_int(const void *lhs, const void *rhs) {
    int a = *(const int *)lhs;
    int b = *(const int *)rhs;
    return (a > b) - (a < b);
}

vs_binary_heap *heap = NULL;
if (vs_binary_heap_create(sizeof(int), cmp_int, NULL, &heap) != VS_STATUS_OK) {
    /* handle allocation failure */
}
```

### vs_binary_heap_push

```c
vs_status vs_binary_heap_push(vs_binary_heap *heap, const void *element);
```

- Parameters: `heap`, `element`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Example:

```c
int value = 42;
if (vs_binary_heap_push(heap, &value) != VS_STATUS_OK) {
    /* handle allocation failure */
}
```

### vs_binary_heap_pop

```c
void *vs_binary_heap_pop(vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to removed top element in heap-managed storage, or `NULL` when empty.
- Example:

```c
int *top = (int *)vs_binary_heap_pop(heap);
if (top != NULL) {
    /* use *top */
}
```

### vs_binary_heap_peek

```c
const void *vs_binary_heap_peek(const vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to top element in heap-managed storage, or `NULL` when empty.
- Example:

```c
const int *top = (const int *)vs_binary_heap_peek(heap);
```

### vs_binary_heap_size

```c
size_t vs_binary_heap_size(const vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: current element count.
- Example:

```c
size_t count = vs_binary_heap_size(heap);
```

### vs_binary_heap_get_iterator

```c
vs_iterator vs_binary_heap_get_iterator(const vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: iterator over the heap's backing-storage order.
- Notes: iteration order is not sorted order. Do not mutate the heap while
  iterating.
- Example:

```c
vs_iterator iter = vs_binary_heap_get_iterator(heap);

const int *item;
while ((item = (const int *)vs_iterator_next(&iter)) != NULL) {
    /* backing-storage order, not sorted order */
}
```

### vs_binary_heap_destroy

```c
void vs_binary_heap_destroy(vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: none.
- Notes: releases heap storage and the opaque handle. Do not use `heap` after this call.
- Example:

```c
vs_binary_heap_destroy(heap);
```
