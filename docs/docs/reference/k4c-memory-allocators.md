# memory.k4c_allocator

## DESCRIPTION

The memory k4c_allocator API provides a generic k4c_allocator interface plus checked
allocation helpers used by `k4c` internals.

Owning APIs follow a construction-time k4c_allocator convention: pass an
k4c_allocator to `*_create`, and the created object copies that allocator value
for later growth and `*_destroy`. Mutation and destroy functions do not take
another k4c_allocator argument because freeing with a different allocator than
the one used for allocation is unsafe. The allocator context referenced by
`ctx` must outlive every object that captures it. Passing `NULL` selects the C
library k4c_heap.

## TYPES

### k4c_allocator

```c
typedef struct k4c_allocator {
    void *ctx;
    unsigned int features;
    const k4c_allocator_vtable *vtable;
} k4c_allocator;
```

- Fields: `ctx`, `features`, `vtable`
- Notes: owning containers copy this value at create time and reuse it for later
  allocation, reallocation, and destroy work.

### k4c_allocator_vtable

```c
typedef struct k4c_allocator_vtable {
    k4c_alloc_fn alloc;
    k4c_resize_fn resize;
    k4c_dealloc_fn dealloc;
} k4c_allocator_vtable;
```

- Fields: `alloc`, `resize`, `dealloc`
- Notes: concrete allocators provide the operations supported by their feature
  flags.

### k4c_allocator_features

```c
typedef enum k4c_allocator_features {
    K4C_ALLOCATOR_FEATURE_DEALLOC = 1 << 0,
    K4C_ALLOCATOR_FEATURE_REALLOC = 1 << 1,
    K4C_ALLOCATOR_FEATURE_RESET = 1 << 2,
} k4c_allocator_features;
```

- `K4C_ALLOCATOR_FEATURE_DEALLOC`: k4c_allocator supports freeing individual allocations.
- `K4C_ALLOCATOR_FEATURE_REALLOC`: k4c_allocator supports resizing allocations.
- `K4C_ALLOCATOR_FEATURE_RESET`: k4c_allocator supports releasing all allocations together.

## FUNCTIONS

### k4c_alloc

```c
k4c_status k4c_alloc(k4c_allocator *k4c_allocator, size_t size, void **out);
```

- Parameters: `k4c_allocator`, `size`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: allocated memory pointer to `*out` on success; `NULL` on failure.
- Notes: when `k4c_allocator` is `NULL`, has no vtable, or has no `alloc`
  operation, uses the C library k4c_heap.

### k4c_allocator_copy

```c
k4c_allocator k4c_allocator_copy(const k4c_allocator *k4c_allocator);
```

- Parameters: `k4c_allocator`
- Returns: a copied allocator value, or the default libc-backed allocator value
  when `k4c_allocator` is `NULL`.
- Notes: this is useful when an owning object needs to retain the allocator
  selected at construction time.

### k4c_resize

```c
k4c_status k4c_resize(k4c_allocator *k4c_allocator, void *ptr, size_t size, void **out);
```

- Parameters: `k4c_allocator`, `ptr`, `size`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: resized memory pointer to `*out` on success; `NULL` when `size == 0` or allocation fails.
- Notes: when `k4c_allocator` is `NULL`, has no vtable, or has no `resize`
  operation, uses the C library k4c_heap. When `size == 0`, deallocates `ptr`.

### k4c_dealloc

```c
void k4c_dealloc(k4c_allocator *k4c_allocator, void *ptr);
```

- Parameters: `k4c_allocator`, `ptr`
- Returns: none.
- Notes: when `k4c_allocator` is `NULL`, uses the C library k4c_heap. If a
  custom k4c_allocator does not advertise `K4C_ALLOCATOR_FEATURE_DEALLOC`, this
  function returns without calling a dealloc operation.

## EXAMPLES

### Use the default allocator

Pass `NULL` when the caller does not need a custom allocation strategy.

```c
#include <k4c/error.h>
#include <k4c/memory/allocator.h>

int main(void) {
    void *data = NULL;
    if (k4c_alloc(NULL, 1024, &data) != K4C_STATUS_OK) {
        return 1;
    }

    k4c_dealloc(NULL, data);
    return 0;
}
```

### Use an arena allocator

Use an arena when many allocations share one lifetime. Individual deallocation
is not supported; reset or destroy the arena when the whole group is done.

```c
#include <k4c/error.h>
#include <k4c/memory/allocator.h>
#include <k4c/memory/arena.h>

int main(void) {
    k4c_arena *arena = NULL;
    if (k4c_arena_create(4096, &arena) != K4C_STATUS_OK) {
        return 1;
    }

    k4c_allocator allocator = k4c_arena_allocator_view(arena);
    void *scratch = NULL;
    if (k4c_alloc(&allocator, 256, &scratch) != K4C_STATUS_OK) {
        k4c_arena_destroy(arena);
        return 1;
    }

    k4c_arena_reset(arena);
    k4c_arena_destroy(arena);
    return 0;
}
```

### Use a heap allocator

Use `k4c_heap` when the caller wants a bounded allocator that supports
allocation, resize, and individual deallocation.

```c
#include <k4c/error.h>
#include <k4c/memory/allocator.h>
#include <k4c/memory/general_heap.h>

int main(void) {
    k4c_heap *heap = NULL;
    if (k4c_heap_create(4096, &heap) != K4C_STATUS_OK) {
        return 1;
    }

    k4c_allocator allocator = k4c_heap_allocator_view(heap);
    void *data = NULL;
    if (k4c_alloc(&allocator, 128, &data) != K4C_STATUS_OK) {
        k4c_heap_destroy(heap);
        return 1;
    }

    void *resized = NULL;
    if (k4c_resize(&allocator, data, 256, &resized) != K4C_STATUS_OK) {
        k4c_dealloc(&allocator, data);
        k4c_heap_destroy(heap);
        return 1;
    }
    data = resized;

    k4c_dealloc(&allocator, data);
    k4c_heap_destroy(heap);
    return 0;
}
```

### Copy an allocator into a handle

Owning objects should copy the allocator value selected at construction time and
use the copied value for later growth and destroy work.

```c
#include <stddef.h>

#include <k4c/error.h>
#include <k4c/memory/allocator.h>

typedef struct scratch_buffer {
    k4c_allocator allocator;
    void *data;
} scratch_buffer;

k4c_status scratch_buffer_create(k4c_allocator *allocator, size_t size, scratch_buffer *out) {
    out->allocator = k4c_allocator_copy(allocator);
    out->data = NULL;

    return k4c_alloc(&out->allocator, size, &out->data);
}

void scratch_buffer_destroy(scratch_buffer *buffer) {
    k4c_dealloc(&buffer->allocator, buffer->data);
    buffer->data = NULL;
}
```

### Use the test allocator

Use `k4c_test_allocator` in tests to assert that allocator-aware APIs release
what they allocate.

```c
#include <k4c/ds/vector.h>
#include <k4c/error.h>
#include <k4c/memory/test_allocator.h>

int main(void) {
    k4c_test_allocator test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&test_allocator);

    k4c_vector *values = NULL;
    if (k4c_vector_create(sizeof(int), &allocator, &values) != K4C_STATUS_OK) {
        return 1;
    }

    int value = 42;
    if (k4c_vector_push(values, &value) != K4C_STATUS_OK) {
        k4c_vector_destroy(values);
        return 1;
    }

    k4c_vector_destroy(values);
    return k4c_test_allocator_is_clean(&test_allocator) ? 0 : 1;
}
```
