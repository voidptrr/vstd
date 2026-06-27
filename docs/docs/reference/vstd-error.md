# error

## DESCRIPTION

The error module defines the shared status type used by fallible `vstd` APIs.
Invalid required arguments are still programmer errors and are asserted; expected
runtime failures, such as allocation failure, are reported with `vs_status`.

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
    VS_STATUS_IO_ERROR,
    VS_STATUS_INVALID_DATA,
    VS_STATUS_UNSUPPORTED,
} vs_status;
```

- `VS_STATUS_OK`: operation succeeded.
- `VS_STATUS_NO_MEMORY`: allocation failed.
- `VS_STATUS_INVALID_DATA`: input data was malformed.
- `VS_STATUS_IO_ERROR`: an I/O operation failed.

## FUNCTIONS

### vs_status_message

```c
const char *vs_status_message(vs_status status);
```

- Parameters: `status`
- Returns: static human-readable message for `status`.
