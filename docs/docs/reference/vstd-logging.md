# vstd.logging

## DESCRIPTION

The logging module provides a small printf-backed global logger with severity levels,
optional ANSI colors, optional C-library timestamps, and configurable prefixes.
Messages include a bracketed level marker such as `[INFO]`, `[WARN]`, `[ERROR]`,
or `[DEBUG]`.

## TYPES

### vs_log_level

```c
typedef enum vs_log_level {
    VS_LOG_LEVEL_DEBUG = 0,
    VS_LOG_LEVEL_INFO = 1,
    VS_LOG_LEVEL_WARN = 2,
    VS_LOG_LEVEL_ERROR = 3,
} vs_log_level;
```

Levels are ordered by severity and are filtered through the global minimum level.

### vs_log_timestamp

```c
typedef enum vs_log_timestamp {
    VS_LOG_TIMESTAMP_NONE = 0,
    VS_LOG_TIMESTAMP_TIME = 1,
    VS_LOG_TIMESTAMP_DATETIME = 2,
    VS_LOG_TIMESTAMP_UNIX = 3,
} vs_log_timestamp;
```

Timestamp values use the C library `time`, `localtime`, and `strftime` APIs.

## FUNCTIONS

### vs_log_set_level

```c
void vs_log_set_level(vs_log_level level);
```

Sets the minimum level to print. Messages below this level are ignored.

### vs_log_set_timestamp

```c
void vs_log_set_timestamp(vs_log_timestamp timestamp);
```

Sets the timestamp format used before each message.

### vs_log_set_prefixes

```c
void vs_log_set_prefixes(const char *message_prefix, const char *error_prefix);
```

Sets the normal and error prefixes. `message_prefix` is used for debug, info,
and warn messages. `error_prefix` is used for error messages. Set either prefix
to `NULL` or an empty string to omit it.

### vs_log_set_color

```c
void vs_log_set_color(bool enabled);
```

Enables or disables ANSI color output.

### vs_log_debug

```c
void vs_log_debug(const char *fmt, ...);
```

Logs a debug message.

### vs_log_info

```c
void vs_log_info(const char *fmt, ...);
```

Logs an info message.

### vs_log_warn

```c
void vs_log_warn(const char *fmt, ...);
```

Logs a warning message.

### vs_log_error

```c
void vs_log_error(const char *fmt, ...);
```

Logs an error message.

## EXAMPLE

```c
#include <vstd/logging.h>

int main(void) {
    vs_log_set_timestamp(VS_LOG_TIMESTAMP_DATETIME);
    vs_log_set_prefixes("app", "app-error");

    vs_log_info("started");
    vs_log_warn("using global logger");
    return 0;
}
```
