# memory.allocators.test_allocator

## DESCRIPTION

The test allocator is a malloc-backed tracking allocator for checking that
allocator-aware APIs allocate, reallocate, and release through the expected
adapter.

It belongs with the memory allocator APIs rather than the assertion helpers in
`ckit/testing.h`. Tests that use it should include both headers when they also
use `CK_TEST_ASSERT*` macros.

## TYPES

### ck_test_allocator

```c
#include <ckit/memory/allocators/test_allocator.h>

typedef struct ck_test_allocator {
    size_t alloc_count;
    size_t realloc_count;
    size_t dealloc_count;
    size_t outstanding_allocations;
    size_t failed_allocations;
    size_t fail_after;
    ck_allocator allocator;
} ck_test_allocator;
```

Tracks allocation events:

- `alloc_count`
- `realloc_count`
- `dealloc_count`
- `outstanding_allocations`
- `failed_allocations`
- `fail_after`

`fail_after` defaults to `CK_TEST_ALLOCATOR_NO_FAILURE`. Setting it to `0`
makes the next allocation or reallocation attempt fail. Setting it to `N` allows
`N` allocation or reallocation attempts before failures begin.

## FUNCTIONS

### ck_test_allocator_init

```c
void ck_test_allocator_init(ck_test_allocator *test_allocator);
```

Initializes a malloc-backed tracking allocator.

### ck_test_allocator_adapter

```c
ck_allocator *ck_test_allocator_adapter(ck_test_allocator *test_allocator);
```

Returns the allocator adapter to pass into ckit APIs.

### ck_test_allocator_reset_counts

```c
void ck_test_allocator_reset_counts(ck_test_allocator *test_allocator);
```

Resets allocation event counters while preserving outstanding allocation state.

### ck_test_allocator_is_clean

```c
bool ck_test_allocator_is_clean(const ck_test_allocator *test_allocator);
```

Returns whether every tracked allocation has been released.

## EXAMPLE

```c
#include <ckit/datastruct/vector.h>
#include <ckit/memory/allocators/test_allocator.h>
#include <ckit/testing.h>

int main(void) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);

    ck_vector *vector = ck_vector_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    int value = 7;

    ck_vector_push(vector, &value);
    CK_TEST_ASSERT_EQ(ck_vector_size(vector), 1);

    ck_vector_destroy(vector);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}
```
