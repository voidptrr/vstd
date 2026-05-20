# datastruct.deque

## DESCRIPTION

The deque module provides a generic circular-buffer deque for fixed-size elements.
It supports front and back insertion/removal and read-only peeks on both ends.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ckit_deque_init

```c
void ckit_deque_init(ckit_deque *deque, size_t elem_size, ckit_allocator *allocator);
```

- Parameters: `deque`, `elem_size`, `allocator`
- Returns: none.
- Notes: when `allocator` is `NULL`, deque uses default `ckit_malloc` backing.

### ckit_deque_push

```c
void ckit_deque_push(ckit_deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: none.

### ckit_deque_pushfront

```c
void ckit_deque_pushfront(ckit_deque *deque, const void *element);
```

- Parameters: `deque`, `element`
- Returns: none.

### ckit_deque_popleft

```c
void *ckit_deque_popleft(ckit_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed front element in deque-managed storage, or `NULL` when empty.

### ckit_deque_popback

```c
void *ckit_deque_popback(ckit_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to removed back element in deque-managed storage, or `NULL` when empty.

### ckit_deque_peekleft

```c
const void *ckit_deque_peekleft(const ckit_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to front element in deque-managed storage, or `NULL` when empty.

### ckit_deque_peekback

```c
const void *ckit_deque_peekback(const ckit_deque *deque);
```

- Parameters: `deque`
- Returns: pointer to back element in deque-managed storage, or `NULL` when empty.

### ckit_deque_free

```c
void ckit_deque_free(ckit_deque *deque);
```

- Parameters: `deque`
- Returns: none.

### ckit_deque_size

```c
size_t ckit_deque_size(const ckit_deque *deque);
```

- Parameters: `deque`
- Returns: current element count.
- Notes: returns `0` when `deque` is `NULL`.

### ckit_deque_is_empty

```c
bool ckit_deque_is_empty(const ckit_deque *deque);
```

- Parameters: `deque`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `deque` is `NULL`.
