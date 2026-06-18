# vstd.assert

## DESCRIPTION

The assert module provides fail-fast runtime assertions and test assertion
helpers.

## RUNTIME ASSERTIONS

### VSTD_ASSERT

```c
#include <vstd/assert.h>

#define VSTD_ASSERT(cond, message)
```

- Parameters: `cond`, `message`
- Behavior: calls `vs_panic(message)` when `cond` is false.

### vs_panic

```c
#include <vstd/assert.h>

_Noreturn void vs_panic(const char *message);
```

- Parameters: `message`
- Behavior: prints `message` when it is not `NULL`, then aborts.

## TEST ASSERTIONS

### VS_ASSERT

```c
#include <vstd/assert.h>

#define VS_ASSERT(condition)
```

- Parameters: `condition`
- Behavior: prints the failed expression with file and line, then returns `1`
  from the current function.

### VS_ASSERT_MSG

```c
#include <vstd/assert.h>

#define VS_ASSERT_MSG(condition, message)
```

- Parameters: `condition`, `message`
- Behavior: like `VS_ASSERT`, with an additional message.

### VS_ASSERT_EQ

```c
#include <vstd/assert.h>

#define VS_ASSERT_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two scalar values using `!=` and prints the failed
  expression with file and line.

### VS_ASSERT_PTR_NULL

```c
#include <vstd/assert.h>

#define VS_ASSERT_PTR_NULL(ptr)
```

- Parameters: `ptr`
- Behavior: verifies that `ptr` is `NULL`.

### VS_ASSERT_PTR_NOT_NULL

```c
#include <vstd/assert.h>

#define VS_ASSERT_PTR_NOT_NULL(ptr)
```

- Parameters: `ptr`
- Behavior: verifies that `ptr` is not `NULL`.

### VS_ASSERT_PTR_EQ

```c
#include <vstd/assert.h>

#define VS_ASSERT_PTR_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two object pointers and prints both addresses on failure.

### VS_ASSERT_PTR_NE

```c
#include <vstd/assert.h>

#define VS_ASSERT_PTR_NE(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: verifies that two object pointers are different.

### VS_ASSERT_STR_EQ

```c
#include <vstd/assert.h>

#define VS_ASSERT_STR_EQ(actual, expected)
```

- Parameters: `actual`, `expected`
- Behavior: compares two C strings and prints both values on failure. Two
  `NULL` strings compare equal.
