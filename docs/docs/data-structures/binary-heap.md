# ds.binary_heap

## DESCRIPTION

The binary_heap module provides a generic binary heap backed by contiguous vector storage.
Heap ordering is defined by a caller callback. When the callback is `NULL`,
ordering is byte-wise with `memcmp` over stored element bytes.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### binary_heap_iterator

```c
typedef struct binary_heap_iterator binary_heap_iterator;
```

Typed cursor for walking heap backing storage.

## FUNCTIONS

### binary_heap_create

```c
status binary_heap_create(size_t elem_size,
                                binary_heap_cmp_fn cmp,
                                allocator *allocator,
                                binary_heap **out);
```

- Parameters: `elem_size`, `cmp`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: opaque binary-heap handle to `*out` on success.
- Notes: the binary heap stores `allocator` and reuses it for growth and
  destroy. When `allocator` is `NULL`, binary heap storage uses the C library
  heap through `alloc`/`resize`. When `cmp` is `NULL`, element ordering
  uses byte comparison.
- Example:

```c
static int cmp_int(const void *lhs, const void *rhs) {
    int a = *(const int *)lhs;
    int b = *(const int *)rhs;
    return (a > b) - (a < b);
}

binary_heap *heap = NULL;
if (binary_heap_create(sizeof(int), cmp_int, NULL, &heap) != STATUS_OK) {
    /* handle allocation failure */
}
```

### binary_heap_push

```c
status binary_heap_push(binary_heap *heap, const void *element);
```

- Parameters: `heap`, `element`
- Returns: `STATUS_OK` on success, or an error status.
- Example:

```c
int value = 42;
if (binary_heap_push(heap, &value) != STATUS_OK) {
    /* handle allocation failure */
}
```

### binary_heap_pop

```c
void *binary_heap_pop(binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to removed top element in heap-managed storage, or `NULL` when empty.
- Example:

```c
int *top = (int *)binary_heap_pop(heap);
if (top != NULL) {
    /* use *top */
}
```

### binary_heap_peek

```c
const void *binary_heap_peek(const binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to top element in heap-managed storage, or `NULL` when empty.
- Example:

```c
const int *top = (const int *)binary_heap_peek(heap);
```

### binary_heap_size

```c
size_t binary_heap_size(const binary_heap *heap);
```

- Parameters: `heap`
- Returns: current element count.
- Example:

```c
size_t count = binary_heap_size(heap);
```

### binary_heap_get_iterator

```c
iterator binary_heap_get_iterator(const binary_heap *heap);
```

- Parameters: `heap`
- Returns: iterator over the heap's backing-storage order.
- Notes: iteration order is not sorted order. Do not mutate the heap while
  iterating.
- Example:

```c
iterator iter = binary_heap_get_iterator(heap);

const int *item;
while ((item = (const int *)iterator_next(&iter)) != NULL) {
    /* backing-storage order, not sorted order */
}
```

### binary_heap_destroy

```c
void binary_heap_destroy(binary_heap *heap);
```

- Parameters: `heap`
- Returns: none.
- Notes: releases heap storage and the opaque handle. Do not use `heap` after this call.
- Example:

```c
binary_heap_destroy(heap);
```
