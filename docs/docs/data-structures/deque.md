# datastruct.deque

## DESCRIPTION

The deque module provides a generic circular-buffer deque for fixed-size elements.
It supports front and back insertion/removal and read-only peeks on both ends.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_deque_iterator

```c
typedef struct vs_deque_iterator vs_deque_iterator;
```

Typed cursor for walking deque elements.

## FUNCTIONS

### vs_deque_create

```c
vs_deque *vs_deque_create(size_t elem_size, vs_allocator *allocator);
```

- Parameters: `elem_size`, `allocator`
- Returns: opaque deque handle.
- Notes: the deque stores `allocator` and reuses it for growth and destroy.
  When `allocator` is `NULL`, deque uses the C library heap through
  `vs_malloc`/`vs_realloc`.
- Example:

```c
vs_deque *deque = vs_deque_create(sizeof(int), NULL);
```

### vs_deque_push

```c
void vs_deque_push(vs_deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: none.
- Example:

```c
int value = 2;
vs_deque_push(deque, &value);
```

### vs_deque_pushfront

```c
void vs_deque_pushfront(vs_deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: none.
- Example:

```c
int value = 1;
vs_deque_pushfront(deque, &value);
```

### vs_deque_popleft

```c
void *vs_deque_popleft(vs_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed front element in deque-managed storage, or `NULL` when empty.
- Example:

```c
int *front = (int *)vs_deque_popleft(deque);
if (front != NULL) {
    /* use *front */
}
```

### vs_deque_popback

```c
void *vs_deque_popback(vs_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed back element in deque-managed storage, or `NULL` when empty.
- Example:

```c
int *back = (int *)vs_deque_popback(deque);
if (back != NULL) {
    /* use *back */
}
```

### vs_deque_peekleft

```c
const void *vs_deque_peekleft(const vs_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to front element in deque-managed storage, or `NULL` when empty.
- Example:

```c
const int *front = (const int *)vs_deque_peekleft(deque);
```

### vs_deque_peekback

```c
const void *vs_deque_peekback(const vs_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to back element in deque-managed storage, or `NULL` when empty.
- Example:

```c
const int *back = (const int *)vs_deque_peekback(deque);
```

### vs_deque_size

```c
size_t vs_deque_size(const vs_deque *deque);
```

- Parameters: `deque`
- Returns: current element count.
- Example:

```c
size_t count = vs_deque_size(deque);
```

### vs_deque_get_iterator

```c
vs_iterator vs_deque_get_iterator(const vs_deque *deque);
```

- Parameters: `deque`
- Returns: iterator over deque elements from front to back.
- Notes: yielded pointers refer to deque-managed storage. Do not mutate the
  deque while iterating.
- Example:

```c
vs_iterator iter = vs_deque_get_iterator(deque);

const int *item;
while ((item = (const int *)vs_iterator_next(&iter)) != NULL) {
    /* use *item */
}
```

### vs_deque_destroy

```c
void vs_deque_destroy(vs_deque *deque);
```

- Parameters: `deque`
- Returns: none.
- Notes: releases deque storage and the opaque handle. Do not use `deque` after this call.
- Example:

```c
vs_deque_destroy(deque);
```
