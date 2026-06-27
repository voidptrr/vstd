# error

## DESCRIPTION

The error module defines the shared status type used by fallible `vstd` APIs.
Invalid required arguments are still programmer errors and are asserted; expected
runtime failures, such as allocation failure, are reported with `status`.
Fallible public APIs are marked with `NODISCARD`, so callers should check or
explicitly discard returned statuses.

## MACROS

### NODISCARD

```c
#define NODISCARD
```

- Notes: expands to a compiler attribute when supported. Used on fallible
  functions that return `status`.

### RETURN_IF_ERROR

```c
#define RETURN_IF_ERROR(expr)
```

- Parameters: `expr`
- Notes: evaluates a `status` expression and returns the status immediately
  when it is not `STATUS_OK`.

## TYPES

### status

```c
typedef enum status {
    STATUS_OK = 0,
    STATUS_INVALID_ARGUMENT,
    STATUS_NO_MEMORY,
    STATUS_OVERFLOW,
    STATUS_OUT_OF_RANGE,
    STATUS_NOT_FOUND,
    STATUS_EOF,
    STATUS_IO,
    STATUS_INVALID_DATA,
    STATUS_UNSUPPORTED,
} status;
```

- `STATUS_OK`: operation succeeded.
- `STATUS_NO_MEMORY`: allocation failed.
- `STATUS_OVERFLOW`: size arithmetic overflowed before an operation could run.
- `STATUS_INVALID_DATA`: input data was malformed.
- `STATUS_IO`: an I/O operation failed.

## FUNCTIONS

### status_message

```c
const char *status_message(status status);
```

- Parameters: `status`
- Returns: static human-readable message for `status`.
