# vstd.testing

## DESCRIPTION

The testing module provides small helpers for C tests that return `0` on success
and non-zero on failure.

Test assertion macros live in `vstd/assert.h`. The malloc-backed tracking
allocator lives in `vstd/memory/allocators/test_allocator.h`.

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

## EXAMPLE

```c
#include <vstd/assert.h>
#include <vstd/testing.h>

VS_TEST(value_matches_expected) {
    int value = 7;
    VS_ASSERT_EQ(value, 7);
    return 0;
}

VS_TEST_MAIN(VS_TEST_CASE(value_matches_expected))
```
