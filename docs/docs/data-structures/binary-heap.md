# datastruct.binary_heap

## DESCRIPTION

The binary_heap module provides a generic binary heap backed by contiguous vector storage.
Heap ordering is defined by a user-supplied comparator function.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ck_binary_heap_init

```c
ck_binary_heap *ck_binary_heap_init(size_t elem_size,
                                        ck_heap_cmp_fn cmp,
                                        ck_allocator *allocator);
```

- Parameters: `elem_size`, `cmp`, `allocator`
- Returns: opaque binary-heap handle.
- Notes: when `allocator` is `NULL`, binary heap storage uses the C library heap through `ck_malloc`/`ck_realloc`.

### ck_binary_heap_push

```c
void ck_binary_heap_push(ck_binary_heap *heap, const void *element);
```

- Parameters: `heap`, `element`
- Returns: none.

### ck_binary_heap_pop

```c
void *ck_binary_heap_pop(ck_binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to removed top element in heap-managed storage, or `NULL` when empty.

### ck_binary_heap_peek

```c
const void *ck_binary_heap_peek(const ck_binary_heap *heap);
```

- Parameters: `heap`
- Returns: pointer to top element in heap-managed storage, or `NULL` when empty.

### ck_binary_heap_size

```c
size_t ck_binary_heap_size(const ck_binary_heap *heap);
```

- Parameters: `heap`
- Returns: current element count.

### ck_binary_heap_deinit

```c
void ck_binary_heap_deinit(ck_binary_heap *heap);
```

- Parameters: `heap`
- Returns: none.
- Notes: releases heap storage and the opaque handle. Do not use `heap` after this call.

## EXAMPLE

```c
#include <ckit/compare.h>
#include <ckit/datastruct/binary_heap.h>
#include <stdint.h>

int main(void) {
    int status = 0;
    ck_binary_heap *heap = ck_binary_heap_init(sizeof(int32_t), ck_cmp_i32, NULL);
    int32_t values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        ck_binary_heap_push(heap, &values[i]);
    }

    const int32_t *top = (const int32_t *)ck_binary_heap_peek(heap);
    if (top == NULL || *top != 1) {
        status = 1;
        goto cleanup;
    }

    int32_t *popped = (int32_t *)ck_binary_heap_pop(heap);
    if (popped == NULL || *popped != 1) {
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_binary_heap_deinit(heap);
    return status;
}
```
