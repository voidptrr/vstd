# datastruct.vector

## DESCRIPTION

The vector module provides a generic contiguous growable array for fixed-size elements.
Elements are copied into vector-owned storage on push.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### vs_vector_create

```c
vs_vector *vs_vector_create(size_t elem_size, vs_allocator *allocator);
```

- Parameters: `elem_size`, `allocator`
- Returns: opaque vector handle.
- Notes: the vector stores `allocator` and reuses it for growth and destroy.
  When `allocator` is `NULL`, vector uses the C library heap through
  `vs_malloc`/`vs_realloc`.

### vs_vector_push

```c
void vs_vector_push(vs_vector *vector, const void *element);
```

- Parameters: `vector`, `element`
- Returns: none.

### vs_vector_pop

```c
void *vs_vector_pop(vs_vector *vector);
```

- Parameters: `vector`
- Returns: pointer to the removed element within vector-managed storage, or `NULL` when the vector is empty.

### vs_vector_get

```c
void *vs_vector_get(vs_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to item at `index`, or `NULL` when out of range.

### vs_vector_get_const

```c
const void *vs_vector_get_const(const vs_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: const pointer to item at `index`, or `NULL` when out of range.

### vs_vector_elem_size

```c
size_t vs_vector_elem_size(const vs_vector *vector);
```

- Parameters: `vector`
- Returns: configured element size.

### vs_vector_swap_remove

```c
void *vs_vector_swap_remove(vs_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to the slot where the removed item lived, or `NULL` when out of range.
- Notes: removal does not preserve order; the last item is moved into `index`.

### vs_vector_size

```c
size_t vs_vector_size(const vs_vector *vector);
```

- Parameters: `vector`
- Returns: current element count.

### vs_vector_destroy

```c
void vs_vector_destroy(vs_vector *vector);
```

- Parameters: `vector`
- Returns: none.
- Notes: releases vector storage and the opaque handle. Do not use `vector` after this call.

## EXAMPLE

```c
#include <vstd/datastruct/vector.h>

int main(void) {
    int status = 0;
    vs_vector *vector = vs_vector_create(sizeof(int), NULL);
    int first = 10;
    int second = 20;

    vs_vector_push(vector, &first);
    vs_vector_push(vector, &second);

    int *item = (int *)vs_vector_get(vector, 1);
    if (item == NULL || *item != second) {
        status = 1;
        goto cleanup;
    }

    int *popped = (int *)vs_vector_pop(vector);
    if (popped == NULL || *popped != second) {
        status = 1;
        goto cleanup;
    }

cleanup:
    vs_vector_destroy(vector);
    return status;
}
```
