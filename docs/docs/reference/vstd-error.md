# error

## DESCRIPTION

The error module defines the shared status type used by fallible `vstd` APIs.
Invalid required arguments are still programmer errors and are asserted; expected
runtime failures, such as allocation failure, are reported with `vs_status`.
Fallible public APIs are marked with `VS_NODISCARD`, so callers should check or
explicitly discard returned statuses.

## MACROS

### VS_NODISCARD

```c
#define VS_NODISCARD
```

- Notes: expands to a compiler attribute when supported. Used on fallible
  functions that return `vs_status`.

### VS_RETURN_IF_ERROR

```c
#define VS_RETURN_IF_ERROR(expr)
```

- Parameters: `expr`
- Notes: evaluates a `vs_status` expression and returns the status immediately
  when it is not `VS_STATUS_OK`.

## TYPES

### vs_status

```c
typedef enum vs_status {
    VS_STATUS_OK = 0,
    VS_STATUS_INVALID_ARGUMENT,
    VS_STATUS_NO_MEMORY,
    VS_STATUS_OVERFLOW,
    VS_STATUS_OUT_OF_RANGE,
    VS_STATUS_NOT_FOUND,
    VS_STATUS_EOF,
    VS_STATUS_IO,
    VS_STATUS_INVALID_DATA,
    VS_STATUS_UNSUPPORTED,
} vs_status;
```

- `VS_STATUS_OK`: operation succeeded.
- `VS_STATUS_NO_MEMORY`: allocation failed.
- `VS_STATUS_OVERFLOW`: size arithmetic overflowed before an operation could run.
- `VS_STATUS_INVALID_DATA`: input data was malformed.
- `VS_STATUS_IO`: an I/O operation failed.

## FUNCTIONS

### vs_status_message

```c
const char *vs_status_message(vs_status status);
```

- Parameters: `status`
- Returns: static human-readable message for `status`.
