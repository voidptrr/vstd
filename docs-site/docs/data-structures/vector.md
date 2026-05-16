# cstd.datastruct.vector

## DESCRIPTION

The vector module provides a generic contiguous growable array for fixed-size elements.
Elements are copied into vector-owned storage on push and copied out on pop.

## FUNCTIONS

### cstd_vector_init

```c
cstd_status cstd_vector_init(cstd_vector *vector, size_t elem_size);
```

- Parameters: `vector`, `elem_size`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `vector` is `NULL`; CSTD_ERR_RANGE if `elem_size == 0`; CSTD_ERR_OOM on allocation failure.

### cstd_vector_push

```c
cstd_status cstd_vector_push(cstd_vector *vector, const void *element);
```

- Parameters: `vector`, `element`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `vector` or `element` is `NULL`; CSTD_ERR_OOM on growth allocation failure.

### cstd_vector_pop

```c
cstd_status cstd_vector_pop(cstd_vector *vector, void *out);
```

- Parameters: `vector`, `out`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `vector` or `out` is `NULL`; CSTD_ERR_EMPTY if the vector is empty.
- Notes: output parameter content is unspecified on failure.

### cstd_vector_free

```c
cstd_status cstd_vector_free(cstd_vector *vector);
```

- Parameters: `vector`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `vector` is `NULL`.

### cstd_vector_size

```c
size_t cstd_vector_size(const cstd_vector *vector);
```

- Parameters: `vector`
- Returns: current element count.
- Notes: returns `0` when `vector` is `NULL`.

### cstd_vector_is_empty

```c
bool cstd_vector_is_empty(const cstd_vector *vector);
```

- Parameters: `vector`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `vector` is `NULL`.

## EXAMPLE

```c
#include <cstd/datastruct/vector.h>
#include <cstd/status.h>
#include <stdint.h>

int main(void) {
    cstd_vector vec;
    uint64_t value = 42;
    uint64_t out = 0;

    if (cstd_vector_init(&vec, sizeof(uint64_t)) != CSTD_OK) {
        return 1;
    }
    if (cstd_vector_push(&vec, &value) != CSTD_OK) {
        cstd_vector_free(&vec);
        return 1;
    }
    if (cstd_vector_pop(&vec, &out) != CSTD_OK) {
        cstd_vector_free(&vec);
        return 1;
    }

    cstd_vector_free(&vec);
    return 0;
}
```

## SEE ALSO

`cstd.status`, `cstd.datastruct`, `cstd.datastruct.benchmarks`
