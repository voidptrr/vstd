# ds.vector

## DESCRIPTION

The vector module provides a generic contiguous growable array for fixed-size elements.
Elements are copied into vector-owned storage on push.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vector_iterator

```c
typedef struct vector_iterator vector_iterator;
```

Typed cursor for walking vector elements.

## FUNCTIONS

### vector_for_each

```c
#define vector_for_each(type, item, vector)
```

- Parameters: `type`, `item`, `vector`
- Notes: declares `const type *item` scoped to the loop body.
- Example:

```c
vector_for_each(int, item, vector) {
    /* use *item */
}
```

### vector_create

```c
status vector_create(size_t elem_size, allocator *allocator, vector **out);
```

- Parameters: `elem_size`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: opaque vector handle to `*out` on success.
- Notes: the vector stores `allocator` and reuses it for growth and destroy.
  When `allocator` is `NULL`, vector uses the C library heap through
  `alloc`/`resize`.
- Example:

```c
vector *vector = NULL;
if (vector_create(sizeof(int), NULL, &vector) != STATUS_OK) {
    /* handle allocation failure */
}
```

### vector_create_with_capacity

```c
status vector_create_with_capacity(size_t elem_size,
                                        size_t capacity,
                                        allocator *allocator,
                                        vector **out);
```

- Parameters: `elem_size`, `capacity`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: opaque vector handle with at least `capacity` slots allocated to `*out`.
- Example:

```c
vector *vector = NULL;
if (vector_create_with_capacity(sizeof(int), 128, NULL, &vector) != STATUS_OK) {
    /* handle allocation failure */
}
```

### vector_reserve

```c
status vector_reserve(vector *vector, size_t capacity);
```

- Parameters: `vector`, `capacity`
- Returns: `STATUS_OK` on success, or an error status.
- Notes: grows backing storage when needed; existing items are preserved.
- Example:

```c
if (vector_reserve(vector, 1024) != STATUS_OK) {
    /* handle allocation failure */
}
```

### vector_push

```c
status vector_push(vector *vector, const void *element);
```

- Parameters: `vector`, `element`
- Returns: `STATUS_OK` on success, or an error status.
- Example:

```c
int value = 42;
if (vector_push(vector, &value) != STATUS_OK) {
    /* handle allocation failure */
}
```

### vector_pop

```c
void *vector_pop(vector *vector);
```

- Parameters: `vector`
- Returns: pointer to the removed element within vector-managed storage, or `NULL` when the vector is empty.
- Example:

```c
int *last = (int *)vector_pop(vector);
if (last != NULL) {
    /* use *last */
}
```

### vector_get

```c
void *vector_get(vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to item at `index`, or `NULL` when out of range.
- Example:

```c
int *item = (int *)vector_get(vector, 0);
if (item != NULL) {
    *item = 10;
}
```

### vector_get_const

```c
const void *vector_get_const(const vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: const pointer to item at `index`, or `NULL` when out of range.
- Example:

```c
const int *item = (const int *)vector_get_const(vector, 0);
if (item != NULL) {
    int value = *item;
}
```

### vector_elem_size

```c
size_t vector_elem_size(const vector *vector);
```

- Parameters: `vector`
- Returns: configured element size.
- Example:

```c
size_t elem_size = vector_elem_size(vector);
```

### vector_capacity

```c
size_t vector_capacity(const vector *vector);
```

- Parameters: `vector`
- Returns: number of elements that fit without another allocation.

### vector_data

```c
void *vector_data(vector *vector);
```

- Parameters: `vector`
- Returns: mutable pointer to vector backing storage.
- Notes: the pointer may be invalidated by later growth.

### vector_data_const

```c
const void *vector_data_const(const vector *vector);
```

- Parameters: `vector`
- Returns: const pointer to vector backing storage.
- Notes: the pointer may be invalidated by later growth.

### vector_swap_remove

```c
void *vector_swap_remove(vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to the slot where the removed item lived, or `NULL` when out of range.
- Notes: removal does not preserve order; the last item is moved into `index`.
- Example:

```c
int *slot = (int *)vector_swap_remove(vector, 0);
if (slot != NULL) {
    /* slot now contains the item moved from the old last position */
}
```

### vector_size

```c
size_t vector_size(const vector *vector);
```

- Parameters: `vector`
- Returns: current element count.
- Example:

```c
size_t count = vector_size(vector);
```

### vector_get_iterator

```c
iterator vector_get_iterator(const vector *vector);
```

- Parameters: `vector`
- Returns: iterator over vector elements from index `0` to `size - 1`.
- Notes: yielded pointers refer to vector-managed storage. Do not mutate the
  vector while iterating.
- Example:

```c
iterator iter = vector_get_iterator(vector);

const int *item;
while ((item = (const int *)iterator_next(&iter)) != NULL) {
    /* use *item */
}
```

### vector_lower_bound

```c
size_t vector_lower_bound(const vector *vector,
                             const void *key,
                             vector_cmp_fn cmp);
```

- Parameters: `vector`, `key`, `cmp`
- Returns: first sorted index whose item is not less than `key`.
- Notes: `vector` must already be sorted according to `cmp`.
- Example:

```c
static int cmp_int(const void *lhs, const void *rhs) {
    int a = *(const int *)lhs;
    int b = *(const int *)rhs;
    return (a > b) - (a < b);
}

int key = 20;
size_t index = vector_lower_bound(vector, &key, cmp_int);
```

### vector_upper_bound

```c
size_t vector_upper_bound(const vector *vector,
                             const void *key,
                             vector_cmp_fn cmp);
```

- Parameters: `vector`, `key`, `cmp`
- Returns: first sorted index whose item is greater than `key`.
- Notes: `vector` must already be sorted according to `cmp`.
- Example:

```c
int key = 20;
size_t index = vector_upper_bound(vector, &key, cmp_int);
```

### vector_binary_search

```c
size_t vector_binary_search(const vector *vector,
                               const void *key,
                               vector_cmp_fn cmp);
```

- Parameters: `vector`, `key`, `cmp`
- Returns: sorted index containing `key`, or `vector_size(vector)` when absent.
- Notes: when duplicates exist, returns the lower-bound index for `key`.
- Example:

```c
int key = 20;
size_t index = vector_binary_search(vector, &key, cmp_int);
if (index != vector_size(vector)) {
    const int *found = (const int *)vector_get_const(vector, index);
}
```

### vector_destroy

```c
void vector_destroy(vector *vector);
```

- Parameters: `vector`
- Returns: none.
- Notes: releases vector storage and the opaque handle. Do not use `vector` after this call.
- Example:

```c
vector_destroy(vector);
```
