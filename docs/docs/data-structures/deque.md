# datastruct.deque

## DESCRIPTION

The deque module provides a generic circular-buffer deque for fixed-size elements.
It supports front and back insertion/removal and read-only peeks on both ends.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

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

### vs_deque_push

```c
void vs_deque_push(vs_deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: none.

### vs_deque_pushfront

```c
void vs_deque_pushfront(vs_deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: none.

### vs_deque_popleft

```c
void *vs_deque_popleft(vs_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed front element in deque-managed storage, or `NULL` when empty.

### vs_deque_popback

```c
void *vs_deque_popback(vs_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed back element in deque-managed storage, or `NULL` when empty.

### vs_deque_peekleft

```c
const void *vs_deque_peekleft(const vs_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to front element in deque-managed storage, or `NULL` when empty.

### vs_deque_peekback

```c
const void *vs_deque_peekback(const vs_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to back element in deque-managed storage, or `NULL` when empty.

### vs_deque_size

```c
size_t vs_deque_size(const vs_deque *deque);
```

- Parameters: `deque`
- Returns: current element count.

### vs_deque_destroy

```c
void vs_deque_destroy(vs_deque *deque);
```

- Parameters: `deque`
- Returns: none.
- Notes: releases deque storage and the opaque handle. Do not use `deque` after this call.

## EXAMPLE

```c
#include <vstd/datastruct/deque.h>

int main(void) {
    int status = 0;
    vs_deque *deque = vs_deque_create(sizeof(int), NULL);
    int front = 1;
    int back = 2;

    vs_deque_pushfront(deque, &front);
    vs_deque_push(deque, &back);

    const int *left = (const int *)vs_deque_peekleft(deque);
    const int *right = (const int *)vs_deque_peekback(deque);
    if (left == NULL || right == NULL || *left != front || *right != back) {
        status = 1;
        goto cleanup;
    }

    int *out = (int *)vs_deque_popleft(deque);
    if (out == NULL || *out != front) {
        status = 1;
        goto cleanup;
    }

cleanup:
    vs_deque_destroy(deque);
    return status;
}
```
