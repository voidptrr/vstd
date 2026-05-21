# datastruct.vector

## DESCRIPTION

The vector module provides a generic contiguous growable array for fixed-size elements.
Elements are copied into vector-owned storage on push.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ckit_vector_init

```c
ckit_vector *ckit_vector_init(size_t elem_size, ckit_allocator *allocator);
```

- Parameters: `elem_size`, `allocator`
- Returns: opaque vector handle.
- Notes: when `allocator` is `NULL`, vector uses the C library heap through `ckit_malloc`/`ckit_realloc`.

### ckit_vector_push

```c
void ckit_vector_push(ckit_vector *vector, const void *element);
```

- Parameters: `vector`, `element`
- Returns: none.

### ckit_vector_pop

```c
void *ckit_vector_pop(ckit_vector *vector);
```

- Parameters: `vector`
- Returns: pointer to the removed element within vector-managed storage, or `NULL` when the vector is empty.

### ckit_vector_get

```c
void *ckit_vector_get(ckit_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to item at `index`, or `NULL` when out of range.

### ckit_vector_get_const

```c
const void *ckit_vector_get_const(const ckit_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: const pointer to item at `index`, or `NULL` when out of range.

### ckit_vector_elem_size

```c
size_t ckit_vector_elem_size(const ckit_vector *vector);
```

- Parameters: `vector`
- Returns: configured element size, or `0` when `vector` is `NULL`.

### ckit_vector_swap_remove

```c
void *ckit_vector_swap_remove(ckit_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to the slot where the removed item lived, or `NULL` when out of range.
- Notes: removal does not preserve order; the last item is moved into `index`.

### ckit_vector_free

```c
void ckit_vector_free(ckit_vector *vector);
```

- Parameters: `vector`
- Returns: none.
- Notes: releases vector storage and the opaque handle. Do not use `vector` after this call.

### ckit_vector_size

```c
size_t ckit_vector_size(const ckit_vector *vector);
```

- Parameters: `vector`
- Returns: current element count.
- Notes: returns `0` when `vector` is `NULL`.

### ckit_vector_is_empty

```c
bool ckit_vector_is_empty(const ckit_vector *vector);
```

- Parameters: `vector`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `vector` is `NULL`.
