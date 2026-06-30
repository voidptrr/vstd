# k4c.testing

## DESCRIPTION

The testing module provides small helpers for C tests that return `0` on success
and non-zero on failure.

Function-style test assertions live in this header. The malloc-backed tracking
k4c_allocator lives in `k4c/allocators/test_allocator.h`.

## ASSERTIONS

All assertion helpers return `0` on success and `1` on failure.

### k4c_test_equal

```c
int k4c_test_equal(intmax_t actual, intmax_t expected);
```

Compares two scalar integer values. Boolean checks can use `true` or `false`
as the expected value.

### k4c_test_not_equal

```c
int k4c_test_not_equal(intmax_t actual, intmax_t expected);
```

Verifies that two scalar integer values differ.

### k4c_test_equal_ptr

```c
int k4c_test_equal_ptr(const void *actual, const void *expected);
```

Compares two pointers.

### k4c_test_not_equal_ptr

```c
int k4c_test_not_equal_ptr(const void *actual, const void *expected);
```

Verifies that two pointers differ.

### k4c_test_null

```c
int k4c_test_null(const void *ptr);
```

Verifies that `ptr` is `NULL`.

### k4c_test_not_null

```c
int k4c_test_not_null(const void *ptr);
```

Verifies that `ptr` is not `NULL`.

### k4c_test_equal_str

```c
int k4c_test_equal_str(const char *actual, const char *expected);
```

Compares two C strings. Two `NULL` strings compare equal.

## K4C_TEST CASES

### K4C_TEST

```c
#include <k4c/testing.h>

#define K4C_TEST(name)
```

- Parameters: `name`
- Behavior: defines a named test-case function that returns `0` on success and
  non-zero on failure.

### K4C_TEST_CASE

```c
#include <k4c/testing.h>

#define K4C_TEST_CASE(name)
```

- Parameters: `name`
- Behavior: creates an entry for a `K4C_TEST(name)` function.

### K4C_TEST_MAIN

```c
#include <k4c/testing.h>

#define K4C_TEST_MAIN(...)
```

- Parameters: one or more `K4C_TEST_CASE(name)` entries.
- Behavior: defines `main`, runs every listed case, prints the failed case name,
  and returns `1` if any case fails.

## EXAMPLE

```c
#include <k4c/testing.h>

K4C_TEST(value_matches_expected) {
    int value = 7;
    if (k4c_test_equal(value, 7) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(K4C_TEST_CASE(value_matches_expected))
```
