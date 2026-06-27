# vstd.testing

## DESCRIPTION

The testing module provides small helpers for C tests that return `0` on success
and non-zero on failure.

Function-style test assertions live in this header. The malloc-backed tracking
allocator lives in `vstd/memory/test_allocator.h`.

## ASSERTIONS

All assertion helpers return `0` on success and `1` on failure.

### test_equal

```c
int test_equal(intmax_t actual, intmax_t expected);
```

Compares two scalar integer values. Boolean checks can use `true` or `false`
as the expected value.

### test_not_equal

```c
int test_not_equal(intmax_t actual, intmax_t expected);
```

Verifies that two scalar integer values differ.

### test_equal_ptr

```c
int test_equal_ptr(const void *actual, const void *expected);
```

Compares two pointers.

### test_not_equal_ptr

```c
int test_not_equal_ptr(const void *actual, const void *expected);
```

Verifies that two pointers differ.

### test_null

```c
int test_null(const void *ptr);
```

Verifies that `ptr` is `NULL`.

### test_not_null

```c
int test_not_null(const void *ptr);
```

Verifies that `ptr` is not `NULL`.

### test_equal_str

```c
int test_equal_str(const char *actual, const char *expected);
```

Compares two C strings. Two `NULL` strings compare equal.

## TEST CASES

### TEST

```c
#include <vstd/testing.h>

#define TEST(name)
```

- Parameters: `name`
- Behavior: defines a named test-case function that returns `0` on success and
  non-zero on failure.

### TEST_CASE

```c
#include <vstd/testing.h>

#define TEST_CASE(name)
```

- Parameters: `name`
- Behavior: creates an entry for a `TEST(name)` function.

### TEST_MAIN

```c
#include <vstd/testing.h>

#define TEST_MAIN(...)
```

- Parameters: one or more `TEST_CASE(name)` entries.
- Behavior: defines `main`, runs every listed case, prints the failed case name,
  and returns `1` if any case fails.

## EXAMPLE

```c
#include <vstd/testing.h>

TEST(value_matches_expected) {
    int value = 7;
    if (test_equal(value, 7) != 0) {
        return 1;
    }
    return 0;
}

TEST_MAIN(TEST_CASE(value_matches_expected))
```
