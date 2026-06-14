# datastruct.vector

## DESCRIPTION

The vector module provides a generic contiguous growable array for fixed-size elements.
Elements are copied into vector-owned storage on push.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ck_vector_create

```c
ck_vector *ck_vector_create(size_t elem_size, ck_allocator *allocator);
```

- Parameters: `elem_size`, `allocator`
- Returns: opaque vector handle.
- Notes: the vector stores `allocator` and reuses it for growth and destroy.
  When `allocator` is `NULL`, vector uses the C library heap through
  `ck_malloc`/`ck_realloc`.

### ck_vector_push

```c
void ck_vector_push(ck_vector *vector, const void *element);
```

- Parameters: `vector`, `element`
- Returns: none.

### ck_vector_pop

```c
void *ck_vector_pop(ck_vector *vector);
```

- Parameters: `vector`
- Returns: pointer to the removed element within vector-managed storage, or `NULL` when the vector is empty.

### ck_vector_get

```c
void *ck_vector_get(ck_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to item at `index`, or `NULL` when out of range.

### ck_vector_get_const

```c
const void *ck_vector_get_const(const ck_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: const pointer to item at `index`, or `NULL` when out of range.

### ck_vector_elem_size

```c
size_t ck_vector_elem_size(const ck_vector *vector);
```

- Parameters: `vector`
- Returns: configured element size.

### ck_vector_swap_remove

```c
void *ck_vector_swap_remove(ck_vector *vector, size_t index);
```

- Parameters: `vector`, `index`
- Returns: pointer to the slot where the removed item lived, or `NULL` when out of range.
- Notes: removal does not preserve order; the last item is moved into `index`.

### ck_vector_size

```c
size_t ck_vector_size(const ck_vector *vector);
```

- Parameters: `vector`
- Returns: current element count.

### ck_vector_destroy

```c
void ck_vector_destroy(ck_vector *vector);
```

- Parameters: `vector`
- Returns: none.
- Notes: releases vector storage and the opaque handle. Do not use `vector` after this call.

## EXAMPLE

```c
#include <ckit/datastruct/vector.h>

int main(void) {
    int status = 0;
    ck_vector *vector = ck_vector_create(sizeof(int), NULL);
    int first = 10;
    int second = 20;

    ck_vector_push(vector, &first);
    ck_vector_push(vector, &second);

    int *item = (int *)ck_vector_get(vector, 1);
    if (item == NULL || *item != second) {
        status = 1;
        goto cleanup;
    }

    int *popped = (int *)ck_vector_pop(vector);
    if (popped == NULL || *popped != second) {
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_vector_destroy(vector);
    return status;
}
```
