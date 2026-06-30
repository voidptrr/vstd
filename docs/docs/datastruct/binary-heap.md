# ds.k4c_binary_heap

## DESCRIPTION

The k4c_binary_heap module provides a generic binary k4c_heap backed by contiguous k4c_vector storage.
Heap ordering is defined by a caller callback. When the callback is `NULL`,
ordering is byte-wise with `memcmp` over stored element bytes.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### k4c_binary_heap_iterator

```c
typedef struct k4c_binary_heap_iterator k4c_binary_heap_iterator;
```

Typed cursor for walking k4c_heap backing storage.

## FUNCTIONS

### k4c_binary_heap_create

```c
k4c_status k4c_binary_heap_create(size_t elem_size,
                                k4c_binary_heap_cmp_fn cmp,
                                k4c_allocator *k4c_allocator,
                                k4c_binary_heap **out);
```

- Parameters: `elem_size`, `cmp`, `k4c_allocator`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: opaque binary-k4c_heap handle to `*out` on success.
- Notes: the binary k4c_heap copies `k4c_allocator` and reuses it for growth and
  destroy. When `k4c_allocator` is `NULL`, binary k4c_heap storage uses the C library
  k4c_heap through `k4c_alloc`/`k4c_resize`. When `cmp` is `NULL`, element ordering
  uses byte comparison.
- Example:

```c
static int cmp_int(const void *lhs, const void *rhs) {
    int a = *(const int *)lhs;
    int b = *(const int *)rhs;
    return (a > b) - (a < b);
}

k4c_binary_heap *k4c_heap = NULL;
if (k4c_binary_heap_create(sizeof(int), cmp_int, NULL, &k4c_heap) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_binary_heap_push

```c
k4c_status k4c_binary_heap_push(k4c_binary_heap *k4c_heap, const void *element);
```

- Parameters: `k4c_heap`, `element`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Example:

```c
int value = 42;
if (k4c_binary_heap_push(k4c_heap, &value) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_binary_heap_pop

```c
void *k4c_binary_heap_pop(k4c_binary_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: pointer to removed top element in k4c_heap-managed storage, or `NULL` when empty.
- Example:

```c
int *top = (int *)k4c_binary_heap_pop(k4c_heap);
if (top != NULL) {
    /* use *top */
}
```

### k4c_binary_heap_peek

```c
const void *k4c_binary_heap_peek(const k4c_binary_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: pointer to top element in k4c_heap-managed storage, or `NULL` when empty.
- Example:

```c
const int *top = (const int *)k4c_binary_heap_peek(k4c_heap);
```

### k4c_binary_heap_size

```c
size_t k4c_binary_heap_size(const k4c_binary_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: current element count.
- Example:

```c
size_t count = k4c_binary_heap_size(k4c_heap);
```

### k4c_binary_heap_get_iterator

```c
k4c_iterator k4c_binary_heap_get_iterator(const k4c_binary_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: k4c_iterator over the k4c_heap's backing-storage order.
- Notes: iteration order is not sorted order. Do not mutate the k4c_heap while
  iterating.
- Example:

```c
k4c_iterator iter = k4c_binary_heap_get_iterator(k4c_heap);

const int *item;
while ((item = (const int *)k4c_iterator_next(&iter)) != NULL) {
    /* backing-storage order, not sorted order */
}
```

### k4c_binary_heap_destroy

```c
void k4c_binary_heap_destroy(k4c_binary_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: none.
- Notes: releases k4c_heap storage and the opaque handle. Do not use `k4c_heap` after this call.
- Example:

```c
k4c_binary_heap_destroy(k4c_heap);
```
