# vstd.testing

## DESCRIPTION

The testing module provides small helpers for C tests that return `0` on success
and non-zero on failure.

It includes assertion macros and failure-reporting helpers. The malloc-backed
tracking allocator lives in `vstd/memory/allocators/test_allocator.h`.

## TEST CASES

### VS_TEST

```c
#include <vstd/testing.h>

#define VS_TEST(name)
```

- Parameters: `name`
- Behavior: defines a named test-case function that returns `0` on success and
  non-zero on failure.

### VS_TEST_CASE

```c
#include <vstd/testing.h>

#define VS_TEST_CASE(name)
```

- Parameters: `name`
- Behavior: creates an entry for a `VS_TEST(name)` function.

### VS_TEST_MAIN

```c
#include <vstd/testing.h>

#define VS_TEST_MAIN(...)
```

- Parameters: one or more `VS_TEST_CASE(name)` entries.
- Behavior: defines `main`, runs every listed case, prints the failed case name,
  and returns `1` if any case fails.

## ASSERTIONS

### VS_TEST_ASSERT

```c
#include <vstd/testing.h>

#define VS_TEST_ASSERT(condition)
```

- Parameters: `condition`
- Behavior: prints the failed expression with file and line, then returns `1`
  from the current function.

### VS_TEST_ASSERT_MSG

```c
#include <vstd/testing.h>

#define VS_TEST_ASSERT_MSG(condition, message)
```

- Parameters: `condition`, `message`
- Behavior: like `VS_TEST_ASSERT`, with an additional message.

### VS_TEST_ASSERT_EQ

```c
#include <vstd/testing.h>

#define VS_TEST_ASSERT_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two scalar values using `!=` and prints the failed
  expression with file and line.

### VS_TEST_ASSERT_PTR_NULL

```c
#include <vstd/testing.h>

#define VS_TEST_ASSERT_PTR_NULL(ptr)
```

- Parameters: `ptr`
- Behavior: verifies that `ptr` is `NULL`.

### VS_TEST_ASSERT_PTR_NOT_NULL

```c
#include <vstd/testing.h>

#define VS_TEST_ASSERT_PTR_NOT_NULL(ptr)
```

- Parameters: `ptr`
- Behavior: verifies that `ptr` is not `NULL`.

### VS_TEST_ASSERT_PTR_EQ

```c
#include <vstd/testing.h>

#define VS_TEST_ASSERT_PTR_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two object pointers and prints both addresses on failure.

### VS_TEST_ASSERT_PTR_NE

```c
#include <vstd/testing.h>

#define VS_TEST_ASSERT_PTR_NE(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: verifies that two object pointers are different.

### VS_TEST_ASSERT_STR_EQ

```c
#include <vstd/testing.h>

#define VS_TEST_ASSERT_STR_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two C strings and prints both values on failure. Two
  `NULL` strings compare equal.

## EXAMPLE

```c
#include <vstd/testing.h>

VS_TEST(value_matches_expected) {
    int value = 7;
    VS_TEST_ASSERT_EQ(value, 7);
    return 0;
}

VS_TEST_MAIN(VS_TEST_CASE(value_matches_expected))
```
