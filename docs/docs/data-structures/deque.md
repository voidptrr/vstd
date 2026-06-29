# ds.k4c_deque

## DESCRIPTION

The k4c_deque module provides a generic circular-buffer k4c_deque for fixed-size elements.
It supports front and back insertion/removal and read-only peeks on both ends.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### k4c_deque_iterator

```c
typedef struct k4c_deque_iterator k4c_deque_iterator;
```

Typed cursor for walking k4c_deque elements.

## FUNCTIONS

### k4c_deque_create

```c
k4c_status k4c_deque_create(size_t elem_size, k4c_allocator *k4c_allocator, k4c_deque **out);
```

- Parameters: `elem_size`, `k4c_allocator`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: opaque k4c_deque handle to `*out` on success.
- Notes: the k4c_deque copies `k4c_allocator` and reuses it for growth and destroy.
  When `k4c_allocator` is `NULL`, k4c_deque uses the C library k4c_heap through
  `k4c_alloc`/`k4c_resize`.
- Example:

```c
k4c_deque *k4c_deque = NULL;
if (k4c_deque_create(sizeof(int), NULL, &k4c_deque) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_deque_push

```c
k4c_status k4c_deque_push(k4c_deque *k4c_deque, const void *element);
```

- Parameters: `k4c_deque`, `element`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Example:

```c
int value = 2;
if (k4c_deque_push(k4c_deque, &value) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_deque_pushfront

```c
k4c_status k4c_deque_pushfront(k4c_deque *k4c_deque, const void *element);
```

- Parameters: `k4c_deque`, `element`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Example:

```c
int value = 1;
if (k4c_deque_pushfront(k4c_deque, &value) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_deque_popleft

```c
void *k4c_deque_popleft(k4c_deque *k4c_deque);
```

- Parameters: `k4c_deque`
- Returns: pointer to removed front element in k4c_deque-managed storage, or `NULL` when empty.
- Example:

```c
int *front = (int *)k4c_deque_popleft(k4c_deque);
if (front != NULL) {
    /* use *front */
}
```

### k4c_deque_popback

```c
void *k4c_deque_popback(k4c_deque *k4c_deque);
```

- Parameters: `k4c_deque`
- Returns: pointer to removed back element in k4c_deque-managed storage, or `NULL` when empty.
- Example:

```c
int *back = (int *)k4c_deque_popback(k4c_deque);
if (back != NULL) {
    /* use *back */
}
```

### k4c_deque_peekleft

```c
const void *k4c_deque_peekleft(const k4c_deque *k4c_deque);
```

- Parameters: `k4c_deque`
- Returns: pointer to front element in k4c_deque-managed storage, or `NULL` when empty.
- Example:

```c
const int *front = (const int *)k4c_deque_peekleft(k4c_deque);
```

### k4c_deque_peekback

```c
const void *k4c_deque_peekback(const k4c_deque *k4c_deque);
```

- Parameters: `k4c_deque`
- Returns: pointer to back element in k4c_deque-managed storage, or `NULL` when empty.
- Example:

```c
const int *back = (const int *)k4c_deque_peekback(k4c_deque);
```

### k4c_deque_size

```c
size_t k4c_deque_size(const k4c_deque *k4c_deque);
```

- Parameters: `k4c_deque`
- Returns: current element count.
- Example:

```c
size_t count = k4c_deque_size(k4c_deque);
```

### k4c_deque_get_iterator

```c
k4c_iterator k4c_deque_get_iterator(const k4c_deque *k4c_deque);
```

- Parameters: `k4c_deque`
- Returns: k4c_iterator over k4c_deque elements from front to back.
- Notes: yielded pointers refer to k4c_deque-managed storage. Do not mutate the
  k4c_deque while iterating.
- Example:

```c
k4c_iterator iter = k4c_deque_get_iterator(k4c_deque);

const int *item;
while ((item = (const int *)k4c_iterator_next(&iter)) != NULL) {
    /* use *item */
}
```

### k4c_deque_destroy

```c
void k4c_deque_destroy(k4c_deque *k4c_deque);
```

- Parameters: `k4c_deque`
- Returns: none.
- Notes: releases k4c_deque storage and the opaque handle. Do not use `k4c_deque` after this call.
- Example:

```c
k4c_deque_destroy(k4c_deque);
```
