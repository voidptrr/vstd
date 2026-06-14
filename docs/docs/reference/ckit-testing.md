# ckit.testing

## DESCRIPTION

The testing module provides small helpers for C tests that return `0` on success
and non-zero on failure.

It includes assertion macros and a malloc-backed tracking allocator for checking
that allocator-aware APIs allocate, reallocate, and release through the expected
adapter.

## ASSERTIONS

### CK_TEST_ASSERT

```c
#include <ckit/testing.h>

#define CK_TEST_ASSERT(condition)
```

- Parameters: `condition`
- Behavior: prints the failed expression with file and line, then returns `1`
  from the current function.

### CK_TEST_ASSERT_MSG

```c
#include <ckit/testing.h>

#define CK_TEST_ASSERT_MSG(condition, message)
```

- Parameters: `condition`, `message`
- Behavior: like `CK_TEST_ASSERT`, with an additional message.

### CK_TEST_ASSERT_EQ

```c
#include <ckit/testing.h>

#define CK_TEST_ASSERT_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two scalar values using `!=` and prints the failed
  expression with file and line.

### CK_TEST_ASSERT_PTR_NULL

```c
#include <ckit/testing.h>

#define CK_TEST_ASSERT_PTR_NULL(ptr)
```

- Parameters: `ptr`
- Behavior: verifies that `ptr` is `NULL`.

### CK_TEST_ASSERT_PTR_NOT_NULL

```c
#include <ckit/testing.h>

#define CK_TEST_ASSERT_PTR_NOT_NULL(ptr)
```

- Parameters: `ptr`
- Behavior: verifies that `ptr` is not `NULL`.

### CK_TEST_ASSERT_PTR_EQ

```c
#include <ckit/testing.h>

#define CK_TEST_ASSERT_PTR_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two object pointers and prints both addresses on failure.

### CK_TEST_ASSERT_PTR_NE

```c
#include <ckit/testing.h>

#define CK_TEST_ASSERT_PTR_NE(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: verifies that two object pointers are different.

### CK_TEST_ASSERT_STR_EQ

```c
#include <ckit/testing.h>

#define CK_TEST_ASSERT_STR_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two C strings and prints both values on failure. Two
  `NULL` strings compare equal.

## TRACKING ALLOCATOR

### ck_test_allocator

```c
#include <ckit/testing.h>

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

### ck_test_allocator_init

```c
void ck_test_allocator_init(ck_test_allocator *test_allocator);
```

Initializes a malloc-backed tracking allocator.

### ck_test_allocator_allocator

```c
ck_allocator *ck_test_allocator_allocator(ck_test_allocator *test_allocator);
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
#include <ckit/testing.h>

int main(void) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);

    ck_vector *vector = ck_vector_create(sizeof(int), ck_test_allocator_allocator(&test_allocator));
    int value = 7;

    ck_vector_push(vector, &value);
    CK_TEST_ASSERT_EQ(ck_vector_size(vector), 1);

    ck_vector_destroy(vector);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}
```
