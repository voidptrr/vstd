# memory.test_allocator

## DESCRIPTION

The test allocator is a malloc-backed tracking allocator for checking that
allocator-aware APIs allocate, reallocate, and release through the expected
adapter.

It belongs with the memory allocator APIs rather than the assertion helpers in
`vstd/testing.h`. Tests that use it should include both headers when they also
use testing helpers.

## TYPES

### test_allocator

```c
#include <vstd/memory/test_allocator.h>

typedef struct test_allocator {
    size_t alloc_count;
    size_t realloc_count;
    size_t dealloc_count;
    size_t outstanding_allocations;
    size_t failed_allocations;
    size_t fail_after;
    allocator allocator;
} test_allocator;
```

Tracks allocation events:

- `alloc_count`
- `realloc_count`
- `dealloc_count`
- `outstanding_allocations`
- `failed_allocations`
- `fail_after`

`fail_after` defaults to `TEST_ALLOCATOR_NO_FAILURE`. Setting it to `0`
makes the next allocation or reallocation attempt fail. Setting it to `N` allows
`N` allocation or reallocation attempts before failures begin.

## FUNCTIONS

### test_allocator_init

```c
allocator *test_allocator_init(test_allocator *test_allocator);
```

Initializes a malloc-backed tracking allocator and returns the allocator adapter
to pass into vstd APIs.

### test_allocator_reset_counts

```c
void test_allocator_reset_counts(test_allocator *test_allocator);
```

Resets allocation event counters while preserving outstanding allocation state.

### test_allocator_is_clean

```c
bool test_allocator_is_clean(const test_allocator *test_allocator);
```

Returns whether every tracked allocation has been released.

## EXAMPLE

```c
#include <vstd/ds/vector.h>
#include <vstd/memory/test_allocator.h>
#include <vstd/testing.h>

int main(void) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);

    vector *vector = NULL;
    if (vector_create(sizeof(int), allocator, &vector) != STATUS_OK) {
        /* handle allocation failure */
    }
    int value = 7;

    if (vector_push(vector, &value) != STATUS_OK) {
        /* handle allocation failure */
    }
    if (test_equal(vector_size(vector), 1) != 0) {
        return 1;
    }

    vector_destroy(vector);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}
```
