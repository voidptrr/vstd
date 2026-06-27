# ds.deque

## DESCRIPTION

The deque module provides a generic circular-buffer deque for fixed-size elements.
It supports front and back insertion/removal and read-only peeks on both ends.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### deque_iterator

```c
typedef struct deque_iterator deque_iterator;
```

Typed cursor for walking deque elements.

## FUNCTIONS

### deque_create

```c
status deque_create(size_t elem_size, allocator *allocator, deque **out);
```

- Parameters: `elem_size`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: opaque deque handle to `*out` on success.
- Notes: the deque stores `allocator` and reuses it for growth and destroy.
  When `allocator` is `NULL`, deque uses the C library heap through
  `alloc`/`resize`.
- Example:

```c
deque *deque = NULL;
if (deque_create(sizeof(int), NULL, &deque) != STATUS_OK) {
    /* handle allocation failure */
}
```

### deque_push

```c
status deque_push(deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: `STATUS_OK` on success, or an error status.
- Example:

```c
int value = 2;
if (deque_push(deque, &value) != STATUS_OK) {
    /* handle allocation failure */
}
```

### deque_pushfront

```c
status deque_pushfront(deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: `STATUS_OK` on success, or an error status.
- Example:

```c
int value = 1;
if (deque_pushfront(deque, &value) != STATUS_OK) {
    /* handle allocation failure */
}
```

### deque_popleft

```c
void *deque_popleft(deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed front element in deque-managed storage, or `NULL` when empty.
- Example:

```c
int *front = (int *)deque_popleft(deque);
if (front != NULL) {
    /* use *front */
}
```

### deque_popback

```c
void *deque_popback(deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed back element in deque-managed storage, or `NULL` when empty.
- Example:

```c
int *back = (int *)deque_popback(deque);
if (back != NULL) {
    /* use *back */
}
```

### deque_peekleft

```c
const void *deque_peekleft(const deque *deque);
```

- Parameters: `deque`
- Returns: pointer to front element in deque-managed storage, or `NULL` when empty.
- Example:

```c
const int *front = (const int *)deque_peekleft(deque);
```

### deque_peekback

```c
const void *deque_peekback(const deque *deque);
```

- Parameters: `deque`
- Returns: pointer to back element in deque-managed storage, or `NULL` when empty.
- Example:

```c
const int *back = (const int *)deque_peekback(deque);
```

### deque_size

```c
size_t deque_size(const deque *deque);
```

- Parameters: `deque`
- Returns: current element count.
- Example:

```c
size_t count = deque_size(deque);
```

### deque_get_iterator

```c
iterator deque_get_iterator(const deque *deque);
```

- Parameters: `deque`
- Returns: iterator over deque elements from front to back.
- Notes: yielded pointers refer to deque-managed storage. Do not mutate the
  deque while iterating.
- Example:

```c
iterator iter = deque_get_iterator(deque);

const int *item;
while ((item = (const int *)iterator_next(&iter)) != NULL) {
    /* use *item */
}
```

### deque_destroy

```c
void deque_destroy(deque *deque);
```

- Parameters: `deque`
- Returns: none.
- Notes: releases deque storage and the opaque handle. Do not use `deque` after this call.
- Example:

```c
deque_destroy(deque);
```
