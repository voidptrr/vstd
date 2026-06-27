# vstd.logging

## DESCRIPTION

The logging module provides a small printf-backed global logger with severity levels,
optional ANSI colors, optional C-library timestamps, and configurable prefixes.
Messages include a bracketed level marker such as `[INFO]`, `[WARN]`, `[ERROR]`,
or `[DEBUG]`.

## TYPES

### log_level

```c
typedef enum log_level {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 3,
} log_level;
```

Levels are ordered by severity and are filtered through the global minimum level.

### log_timestamp

```c
typedef enum log_timestamp {
    LOG_TIMESTAMP_NONE = 0,
    LOG_TIMESTAMP_TIME = 1,
    LOG_TIMESTAMP_DATETIME = 2,
    LOG_TIMESTAMP_UNIX = 3,
} log_timestamp;
```

Timestamp values use the C library `time`, `localtime`, and `strftime` APIs.

## FUNCTIONS

### log_set_level

```c
void log_set_level(log_level level);
```

Sets the minimum level to print. Messages below this level are ignored.

### log_set_timestamp

```c
void log_set_timestamp(log_timestamp timestamp);
```

Sets the timestamp format used before each message.

### log_set_prefixes

```c
void log_set_prefixes(const char *message_prefix, const char *error_prefix);
```

Sets the normal and error prefixes. `message_prefix` is used for debug, info,
and warn messages. `error_prefix` is used for error messages. Set either prefix
to `NULL` or an empty string to omit it.

### log_set_color

```c
void log_set_color(bool enabled);
```

Enables or disables ANSI color output.

### log_debug

```c
void log_debug(const char *fmt, ...);
```

Logs a debug message.

### log_info

```c
void log_info(const char *fmt, ...);
```

Logs an info message.

### log_warn

```c
void log_warn(const char *fmt, ...);
```

Logs a warning message.

### log_error

```c
void log_error(const char *fmt, ...);
```

Logs an error message.

## EXAMPLE

```c
#include <vstd/logging.h>

int main(void) {
    log_set_timestamp(LOG_TIMESTAMP_DATETIME);
    log_set_prefixes("app", "app-error");

    log_info("started");
    log_warn("using global logger");
    return 0;
}
```
