# datastruct.binary_heap

## DESCRIPTION

The binary_heap module provides a generic binary heap backed by contiguous vector storage.
Heap ordering is defined by a caller callback. When the callback is `NULL`,
ordering is byte-wise with `memcmp` over stored element bytes.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### vs_binary_heap_create

```c
vs_binary_heap *vs_binary_heap_create(size_t elem_size,
                                        vs_binary_heap_cmp_fn cmp,
                                        vs_allocator *allocator);
```

- Parameters: `elem_size`, `cmp`, `allocator`
- Returns: opaque binary-heap handle.
- Notes: the binary heap stores `allocator` and reuses it for growth and
  destroy. When `allocator` is `NULL`, binary heap storage uses the C library
  heap through `vs_malloc`/`vs_realloc`. When `cmp` is `NULL`, element ordering
  uses byte comparison.

### vs_binary_heap_push

```c
void vs_binary_heap_push(vs_binary_heap *heap, const void *element);
```

- Parameters: `heap`, `element`
- Returns: none.

### vs_binary_heap_pop

```c
void *vs_binary_heap_pop(vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to removed top element in heap-managed storage, or `NULL` when empty.

### vs_binary_heap_peek

```c
const void *vs_binary_heap_peek(const vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to top element in heap-managed storage, or `NULL` when empty.

### vs_binary_heap_size

```c
size_t vs_binary_heap_size(const vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: current element count.

### vs_binary_heap_destroy

```c
void vs_binary_heap_destroy(vs_binary_heap *heap);
```

- Parameters: `heap`
- Returns: none.
- Notes: releases heap storage and the opaque handle. Do not use `heap` after this call.

## EXAMPLE

```c
#include <vstd/datastruct/binary_heap.h>
#include <stdint.h>

int main(void) {
    int status = 0;
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(uint8_t), NULL, NULL);
    uint8_t values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    const uint8_t *top = (const uint8_t *)vs_binary_heap_peek(heap);
    if (top == NULL || *top != 1) {
        status = 1;
        goto cleanup;
    }

    uint8_t *popped = (uint8_t *)vs_binary_heap_pop(heap);
    if (popped == NULL || *popped != 1) {
        status = 1;
        goto cleanup;
    }

cleanup:
    vs_binary_heap_destroy(heap);
    return status;
}
```
