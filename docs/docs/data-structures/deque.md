# datastruct.deque

## DESCRIPTION

The deque module provides a generic circular-buffer deque for fixed-size elements.
It supports front and back insertion/removal and read-only peeks on both ends.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ck_deque_create

```c
ck_deque *ck_deque_create(size_t elem_size, ck_allocator *allocator);
```

- Parameters: `elem_size`, `allocator`
- Returns: opaque deque handle.
- Notes: the deque stores `allocator` and reuses it for growth and destroy.
  When `allocator` is `NULL`, deque uses the C library heap through
  `ck_malloc`/`ck_realloc`.

### ck_deque_push

```c
void ck_deque_push(ck_deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: none.

### ck_deque_pushfront

```c
void ck_deque_pushfront(ck_deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: none.

### ck_deque_popleft

```c
void *ck_deque_popleft(ck_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed front element in deque-managed storage, or `NULL` when empty.

### ck_deque_popback

```c
void *ck_deque_popback(ck_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed back element in deque-managed storage, or `NULL` when empty.

### ck_deque_peekleft

```c
const void *ck_deque_peekleft(const ck_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to front element in deque-managed storage, or `NULL` when empty.

### ck_deque_peekback

```c
const void *ck_deque_peekback(const ck_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to back element in deque-managed storage, or `NULL` when empty.

### ck_deque_size

```c
size_t ck_deque_size(const ck_deque *deque);
```

- Parameters: `deque`
- Returns: current element count.

### ck_deque_destroy

```c
void ck_deque_destroy(ck_deque *deque);
```

- Parameters: `deque`
- Returns: none.
- Notes: releases deque storage and the opaque handle. Do not use `deque` after this call.

## EXAMPLE

```c
#include <ckit/datastruct/deque.h>

int main(void) {
    int status = 0;
    ck_deque *deque = ck_deque_create(sizeof(int), NULL);
    int front = 1;
    int back = 2;

    ck_deque_pushfront(deque, &front);
    ck_deque_push(deque, &back);

    const int *left = (const int *)ck_deque_peekleft(deque);
    const int *right = (const int *)ck_deque_peekback(deque);
    if (left == NULL || right == NULL || *left != front || *right != back) {
        status = 1;
        goto cleanup;
    }

    int *out = (int *)ck_deque_popleft(deque);
    if (out == NULL || *out != front) {
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_deque_destroy(deque);
    return status;
}
```
