# datastruct.vector

## DESCRIPTION

The vector module provides a generic contiguous growable array for fixed-size elements.
Elements are copied into vector-owned storage on push and copied out on pop.

## FUNCTIONS

### ckit_vector_init

```c
ckit_status ckit_vector_init(ckit_vector *vector, size_t elem_size, ckit_allocator *allocator);
```

- Parameters: `vector`, `elem_size`, `allocator`
- Returns: CKIT_OK on success.
- Errors: CKIT_ERR_NULL if `vector` is `NULL`; CKIT_ERR_RANGE if `elem_size == 0`.
- Notes: when `allocator` is `NULL`, vector uses default `ckit_malloc`/`ckit_realloc` backing.

### ckit_vector_push

```c
ckit_status ckit_vector_push(ckit_vector *vector, const void *element);
```

- Parameters: `vector`, `element`
- Returns: CKIT_OK on success.
- Errors: CKIT_ERR_NULL if `vector` or `element` is `NULL`.

### ckit_vector_pop

```c
ckit_status ckit_vector_pop(ckit_vector *vector, void *out);
```

- Parameters: `vector`, `out`
- Returns: CKIT_OK on success.
- Errors: CKIT_ERR_NULL if `vector` or `out` is `NULL`; CKIT_ERR_EMPTY if the vector is empty.
- Notes: output parameter content is unspecified on failure.

### ckit_vector_free

```c
ckit_status ckit_vector_free(ckit_vector *vector);
```

- Parameters: `vector`
- Returns: CKIT_OK on success.
- Errors: CKIT_ERR_NULL if `vector` is `NULL`.

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
