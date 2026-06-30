# memory.general_heap

## DESCRIPTION

The k4c_heap module provides an in-process free-list k4c_allocator backed by an
internal contiguous region. It supports allocation, deallocation, and reallocation
using a coalescing free-list strategy.

This API is fail-fast for required initialization/teardown preconditions.

## TYPES

### k4c_heap

```c
typedef struct k4c_heap k4c_heap;
```

- Notes: `k4c_heap` is opaque. Use the functions below to inspect or mutate
  k4c_heap state.

## FUNCTIONS

### k4c_heap_create

```c
k4c_status k4c_heap_create(size_t capacity, k4c_heap **out);
```

- Parameters: `capacity`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: k4c_heap pointer to `*out` on success.

### k4c_heap_allocator_view

```c
k4c_allocator k4c_heap_allocator_view(k4c_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: generic k4c_allocator value for `k4c_heap`.
- Notes: the returned k4c_allocator advertises `K4C_ALLOCATOR_FEATURE_DEALLOC | K4C_ALLOCATOR_FEATURE_REALLOC`.

### k4c_heap_capacity

```c
size_t k4c_heap_capacity(const k4c_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: total managed bytes.

### k4c_heap_available

```c
size_t k4c_heap_available(const k4c_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: sum of currently free payload bytes.

### k4c_heap_destroy

```c
void k4c_heap_destroy(k4c_heap *k4c_heap);
```

- Parameters: `k4c_heap`
- Returns: none.
- Behavior: releases the backing buffer and k4c_heap handle.

## EXAMPLES

### Allocate And Free

Use `k4c_heap_allocator_view` to allocate through the generic allocator API. The
heap allocator supports individual deallocation.

```c
#include <k4c/error.h>
#include <k4c/allocators/allocator.h>
#include <k4c/allocators/general_heap.h>

int main(void) {
    k4c_heap *heap = NULL;
    if (k4c_heap_create(4096, &heap) != K4C_STATUS_OK) {
        return 1;
    }

    size_t capacity = k4c_heap_capacity(heap);
    size_t before = k4c_heap_available(heap);

    k4c_allocator allocator = k4c_heap_allocator_view(heap);
    int *value = NULL;
    if (k4c_alloc(&allocator, sizeof(*value), (void **)&value) != K4C_STATUS_OK) {
        k4c_heap_destroy(heap);
        return 1;
    }

    *value = 42;
    size_t after = k4c_heap_available(heap);

    if (capacity == 0 || after >= before) {
        k4c_dealloc(&allocator, value);
        k4c_heap_destroy(heap);
        return 1;
    }

    k4c_dealloc(&allocator, value);
    k4c_heap_destroy(heap);
    return 0;
}
```

### Resize An Allocation

```c
#include <string.h>

#include <k4c/error.h>
#include <k4c/allocators/allocator.h>
#include <k4c/allocators/general_heap.h>

int main(void) {
    k4c_heap *heap = NULL;
    if (k4c_heap_create(4096, &heap) != K4C_STATUS_OK) {
        return 1;
    }

    k4c_allocator allocator = k4c_heap_allocator_view(heap);
    char *text = NULL;
    if (k4c_alloc(&allocator, 16, (void **)&text) != K4C_STATUS_OK) {
        k4c_heap_destroy(heap);
        return 1;
    }

    memcpy(text, "hello", 6);

    char *grown = NULL;
    if (k4c_resize(&allocator, text, 64, (void **)&grown) != K4C_STATUS_OK) {
        k4c_dealloc(&allocator, text);
        k4c_heap_destroy(heap);
        return 1;
    }

    k4c_dealloc(&allocator, grown);
    k4c_heap_destroy(heap);
    return 0;
}
```
