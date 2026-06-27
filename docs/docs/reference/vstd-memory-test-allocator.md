# memory.test_allocator

## DESCRIPTION

The test allocator is a malloc-backed tracking allocator for checking that
allocator-aware APIs allocate, reallocate, and release through the expected
adapter.

It belongs with the memory allocator APIs rather than the assertion helpers in
`vstd/testing.h`. Tests that use it should include both headers when they also
use testing helpers.

## TYPES

### vs_test_allocator

```c
#include <vstd/memory/test_allocator.h>

typedef struct vs_test_allocator {
    size_t alloc_count;
    size_t realloc_count;
    size_t dealloc_count;
    size_t outstanding_allocations;
    size_t failed_allocations;
    size_t fail_after;
    vs_allocator allocator;
} vs_test_allocator;
```

Tracks allocation events:

- `alloc_count`
- `realloc_count`
- `dealloc_count`
- `outstanding_allocations`
- `failed_allocations`
- `fail_after`

`fail_after` defaults to `VS_TEST_ALLOCATOR_NO_FAILURE`. Setting it to `0`
makes the next allocation or reallocation attempt fail. Setting it to `N` allows
`N` allocation or reallocation attempts before failures begin.

## FUNCTIONS

### vs_test_allocator_init

```c
vs_allocator *vs_test_allocator_init(vs_test_allocator *test_allocator);
```

Initializes a malloc-backed tracking allocator and returns the allocator adapter
to pass into vstd APIs.

### vs_test_allocator_reset_counts

```c
void vs_test_allocator_reset_counts(vs_test_allocator *test_allocator);
```

Resets allocation event counters while preserving outstanding allocation state.

### vs_test_allocator_is_clean

```c
bool vs_test_allocator_is_clean(const vs_test_allocator *test_allocator);
```

Returns whether every tracked allocation has been released.

## EXAMPLE

```c
#include <vstd/datastruct/vector.h>
#include <vstd/memory/test_allocator.h>
#include <vstd/testing.h>

int main(void) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);

    vs_vector *vector = NULL;
    if (vs_vector_create(sizeof(int), allocator, &vector) != VS_STATUS_OK) {
        /* handle allocation failure */
    }
    int value = 7;

    if (vs_vector_push(vector, &value) != VS_STATUS_OK) {
        /* handle allocation failure */
    }
    if (vs_test_equal(vs_vector_size(vector), 1) != 0) {
        return 1;
    }

    vs_vector_destroy(vector);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}
```
