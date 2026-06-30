# memory.k4c_test_allocator

## DESCRIPTION

The test k4c_allocator is a malloc-backed tracking k4c_allocator for checking that
k4c_allocator-aware APIs allocate, reallocate, and release through the expected
allocator view.

It belongs with the memory k4c_allocator APIs rather than the assertion helpers in
`k4c/testing.h`. Tests that use it should include both headers when they also
use testing helpers.

## TYPES

### k4c_test_allocator

```c
#include <k4c/allocators/test_allocator.h>

typedef struct k4c_test_allocator {
    size_t alloc_count;
    size_t realloc_count;
    size_t dealloc_count;
    size_t outstanding_allocations;
    size_t failed_allocations;
    size_t fail_after;
} k4c_test_allocator;
```

Tracks allocation events:

- `alloc_count`
- `realloc_count`
- `dealloc_count`
- `outstanding_allocations`
- `failed_allocations`
- `fail_after`

`fail_after` defaults to `K4C_TEST_ALLOCATOR_NO_FAILURE`. Setting it to `0`
makes the next allocation or reallocation attempt fail. Setting it to `N` allows
`N` allocation or reallocation attempts before failures begin.

## FUNCTIONS

### k4c_test_allocator_init

```c
k4c_allocator k4c_test_allocator_init(k4c_test_allocator *k4c_test_allocator);
```

Initializes a malloc-backed tracking k4c_allocator and returns the k4c_allocator
value to pass into k4c APIs.

### k4c_test_allocator_reset_counts

```c
void k4c_test_allocator_reset_counts(k4c_test_allocator *k4c_test_allocator);
```

Resets allocation event counters while preserving outstanding allocation state.

### k4c_test_allocator_is_clean

```c
bool k4c_test_allocator_is_clean(const k4c_test_allocator *k4c_test_allocator);
```

Returns whether every tracked allocation has been released.

## EXAMPLE

```c
#include <k4c/datastruct/vector.h>
#include <k4c/error.h>
#include <k4c/allocators/test_allocator.h>
#include <k4c/testing.h>

int main(void) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);

    k4c_vector *k4c_vector = NULL;
    if (k4c_vector_create(sizeof(int), &k4c_allocator, &k4c_vector) != K4C_STATUS_OK) {
        return 1;
    }
    int value = 7;

    if (k4c_vector_push(k4c_vector, &value) != K4C_STATUS_OK) {
        k4c_vector_destroy(k4c_vector);
        return 1;
    }
    if (k4c_test_equal(k4c_vector_size(k4c_vector), 1) != 0) {
        return 1;
    }

    k4c_vector_destroy(k4c_vector);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}
```
