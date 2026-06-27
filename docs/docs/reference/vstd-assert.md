# vstd.assert

## DESCRIPTION

The assert module provides fail-fast runtime assertions.

## RUNTIME ASSERTIONS

### ASSERT

```c
#include <vstd/assert.h>

#define ASSERT(cond, message)
```

- Parameters: `cond`, `message`
- Behavior: calls `panic(message)` when `cond` is false.

### panic

```c
#include <vstd/assert.h>

_Noreturn void panic(const char *message);
```

- Parameters: `message`
- Behavior: prints `message` when it is not `NULL`, then aborts.
