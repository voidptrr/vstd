# cstd.datastruct.binary_heap

## DESCRIPTION

The binary_heap module provides a generic binary heap backed by a contiguous array (`root`).
Heap ordering is defined by a user-supplied comparator function.

## FUNCTIONS

### cstd_binary_heap_init

```c
cstd_status cstd_binary_heap_init(cstd_binary_heap *heap, size_t elem_size, cstd_heap_cmp_fn cmp);
```

- Parameters: `heap`, `elem_size`, `cmp`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `heap` or `cmp` is `NULL`; CSTD_ERR_RANGE if `elem_size == 0`.

### cstd_binary_heap_push

```c
cstd_status cstd_binary_heap_push(cstd_binary_heap *heap, const void *element);
```

- Parameters: `heap`, `element`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `heap` or `element` is `NULL`.

### cstd_binary_heap_pop

```c
cstd_status cstd_binary_heap_pop(cstd_binary_heap *heap, void *out);
```

- Parameters: `heap`, `out`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `heap` or `out` is `NULL`; CSTD_ERR_EMPTY if heap is empty.
- Notes: output parameter content is unspecified on failure.

### cstd_binary_heap_peek

```c
cstd_status cstd_binary_heap_peek(const cstd_binary_heap *heap, void *out);
```

- Parameters: `heap`, `out`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `heap` or `out` is `NULL`; CSTD_ERR_EMPTY if heap is empty.
- Notes: output parameter content is unspecified on failure.

### cstd_binary_heap_free

```c
cstd_status cstd_binary_heap_free(cstd_binary_heap *heap);
```

- Parameters: `heap`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `heap` is `NULL`.

### cstd_binary_heap_size

```c
size_t cstd_binary_heap_size(const cstd_binary_heap *heap);
```

- Parameters: `heap`
- Returns: current element count.
- Notes: returns `0` when `heap` is `NULL`.

### cstd_binary_heap_is_empty

```c
bool cstd_binary_heap_is_empty(const cstd_binary_heap *heap);
```

- Parameters: `heap`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `heap` is `NULL`.

## EXAMPLE

```c
#include <cstd/datastruct/binary_heap.h>
#include <cstd/status.h>
#include <stdint.h>

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    cstd_binary_heap heap;
    int values[] = {5, 1, 7};
    int out = 0;

    if (cstd_binary_heap_init(&heap, sizeof(int), cmp_int_asc) != CSTD_OK) {
        return 1;
    }
    cstd_binary_heap_push(&heap, &values[0]);
    cstd_binary_heap_push(&heap, &values[1]);
    cstd_binary_heap_push(&heap, &values[2]);
    cstd_binary_heap_pop(&heap, &out);
    cstd_binary_heap_free(&heap);

    return 0;
}
```

## SEE ALSO

`cstd.status`, `cstd.datastruct`, `cstd.datastruct.benchmarks`
