# datastruct.vector

## DESCRIPTION

The vector module provides a generic contiguous growable array for fixed-size elements.
Elements are copied into vector-owned storage on push.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_vector_iterator

```c
typedef struct vs_vector_iterator vs_vector_iterator;
```

Typed cursor for walking vector elements.

## FUNCTIONS

### VS_VECTOR_PUSH_AS

```c
#define VS_VECTOR_PUSH_AS(vector, type, value)
```

- Parameters: `vector`, `type`, `value`
- Notes: creates a temporary `type` value and pushes it by address.
- Example:

```c
VS_VECTOR_PUSH_AS(vector, int, 42);
```

### VS_VECTOR_FOR_EACH

```c
#define VS_VECTOR_FOR_EACH(type, item, vector)
```

- Parameters: `type`, `item`, `vector`
- Notes: declares `const type *item` scoped to the loop body.
- Example:

```c
VS_VECTOR_FOR_EACH(int, item, vector) {
    /* use *item */
}
```

### vs_vector_create

```c
vs_vector *vs_vector_create(size_t elem_size, vs_allocator *allocator);
```

- Parameters: `elem_size`, `allocator`
- Returns: opaque vector handle.
- Notes: the vector stores `allocator` and reuses it for growth and destroy.
  When `allocator` is `NULL`, vector uses the C library heap through
  `vs_malloc`/`vs_realloc`.
- Example:

```c
vs_vector *vector = vs_vector_create(sizeof(int), NULL);
```

### vs_vector_create_with_capacity

```c
vs_vector *vs_vector_create_with_capacity(size_t elem_size,
                                          size_t capacity,
                                          vs_allocator *allocator);
```

- Parameters: `elem_size`, `capacity`, `allocator`
- Returns: opaque vector handle with at least `capacity` slots allocated.
- Example:

```c
vs_vector *vector = vs_vector_create_with_capacity(sizeof(int), 128, NULL);
```

### vs_vector_reserve

```c
void vs_vector_reserve(vs_vector *vector, size_t capacity);
```

- Parameters: `vector`, `capacity`
- Returns: none.
- Notes: grows backing storage when needed; existing items are preserved.
- Example:

```c
vs_vector_reserve(vector, 1024);
```

### vs_vector_push

```c
void vs_vector_push(vs_vector *vector, const void *element);
```

- Parameters: `vector`, `element`
- Returns: none.
- Example:

```c
int value = 42;
vs_vector_push(vector, &value);
```

### vs_vector_pop

```c
void *vs_vector_pop(vs_vector *vector);
```

- Parameters: `vector`
- Returns: pointer to the removed element within vector-managed storage, or `NULL` when the vector is empty.
- Example:

```c
int *last = (int *)vs_vector_pop(vector);
if (last != NULL) {
    /* use *last */
}
```

### vs_vector_get

```c
void *vs_vector_get(vs_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to item at `index`, or `NULL` when out of range.
- Example:

```c
int *item = (int *)vs_vector_get(vector, 0);
if (item != NULL) {
    *item = 10;
}
```

### vs_vector_get_const

```c
const void *vs_vector_get_const(const vs_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: const pointer to item at `index`, or `NULL` when out of range.
- Example:

```c
const int *item = (const int *)vs_vector_get_const(vector, 0);
if (item != NULL) {
    int value = *item;
}
```

### vs_vector_elem_size

```c
size_t vs_vector_elem_size(const vs_vector *vector);
```

- Parameters: `vector`
- Returns: configured element size.
- Example:

```c
size_t elem_size = vs_vector_elem_size(vector);
```

### vs_vector_capacity

```c
size_t vs_vector_capacity(const vs_vector *vector);
```

- Parameters: `vector`
- Returns: number of elements that fit without another allocation.

### vs_vector_data

```c
void *vs_vector_data(vs_vector *vector);
```

- Parameters: `vector`
- Returns: mutable pointer to vector backing storage.
- Notes: the pointer may be invalidated by later growth.

### vs_vector_data_const

```c
const void *vs_vector_data_const(const vs_vector *vector);
```

- Parameters: `vector`
- Returns: const pointer to vector backing storage.
- Notes: the pointer may be invalidated by later growth.

### vs_vector_swap_remove

```c
void *vs_vector_swap_remove(vs_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to the slot where the removed item lived, or `NULL` when out of range.
- Notes: removal does not preserve order; the last item is moved into `index`.
- Example:

```c
int *slot = (int *)vs_vector_swap_remove(vector, 0);
if (slot != NULL) {
    /* slot now contains the item moved from the old last position */
}
```

### vs_vector_size

```c
size_t vs_vector_size(const vs_vector *vector);
```

- Parameters: `vector`
- Returns: current element count.
- Example:

```c
size_t count = vs_vector_size(vector);
```

### vs_vector_get_iterator

```c
vs_iterator vs_vector_get_iterator(const vs_vector *vector);
```

- Parameters: `vector`
- Returns: iterator over vector elements from index `0` to `size - 1`.
- Notes: yielded pointers refer to vector-managed storage. Do not mutate the
  vector while iterating.
- Example:

```c
vs_iterator iter = vs_vector_get_iterator(vector);

const int *item;
while ((item = (const int *)vs_iterator_next(&iter)) != NULL) {
    /* use *item */
}
```

### vs_vector_lower_bound

```c
size_t vs_vector_lower_bound(const vs_vector *vector,
                             const void *key,
                             vs_vector_cmp_fn cmp);
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
size_t index = vs_vector_lower_bound(vector, &key, cmp_int);
```

### vs_vector_upper_bound

```c
size_t vs_vector_upper_bound(const vs_vector *vector,
                             const void *key,
                             vs_vector_cmp_fn cmp);
```

- Parameters: `vector`, `key`, `cmp`
- Returns: first sorted index whose item is greater than `key`.
- Notes: `vector` must already be sorted according to `cmp`.
- Example:

```c
int key = 20;
size_t index = vs_vector_upper_bound(vector, &key, cmp_int);
```

### vs_vector_binary_search

```c
size_t vs_vector_binary_search(const vs_vector *vector,
                               const void *key,
                               vs_vector_cmp_fn cmp);
```

- Parameters: `vector`, `key`, `cmp`
- Returns: sorted index containing `key`, or `vs_vector_size(vector)` when absent.
- Notes: when duplicates exist, returns the lower-bound index for `key`.
- Example:

```c
int key = 20;
size_t index = vs_vector_binary_search(vector, &key, cmp_int);
if (index != vs_vector_size(vector)) {
    const int *found = (const int *)vs_vector_get_const(vector, index);
}
```

### vs_vector_destroy

```c
void vs_vector_destroy(vs_vector *vector);
```

- Parameters: `vector`
- Returns: none.
- Notes: releases vector storage and the opaque handle. Do not use `vector` after this call.
- Example:

```c
vs_vector_destroy(vector);
```
