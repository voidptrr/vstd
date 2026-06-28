# buffer.k4c_buf_cursor

## DESCRIPTION

The k4c_buf_cursor module provides a small by-value cursor for walking a
caller-owned byte buffer. It is useful for parsers, tokenizers, and protocol
readers that need to peek, consume, skip, or match bytes without copying input.

The cursor does not own the buffer and does not allocate. The caller must keep
the underlying buffer alive while the cursor is used.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### k4c_buf_cursor

```c
typedef struct k4c_buf_cursor {
    const uint8_t *data;
    size_t len;
    size_t pos;
} k4c_buf_cursor;
```

Cursor over `data[0..len)`. `pos` is the current zero-based byte offset.

### k4c_buf_cursor_predicate_fn

```c
typedef bool (*k4c_buf_cursor_predicate_fn)(void *context, uint8_t byte);
```

Predicate callback used by `k4c_buf_cursor_skip_while`. Return `true` to skip
`byte`, or `false` to stop.

## FUNCTIONS

### k4c_buf_cursor_create

```c
k4c_buf_cursor k4c_buf_cursor_create(const void *data, size_t len);
```

- Parameters: `data`, `len`
- Returns: cursor over `data[0..len)`.
- Notes: `data` may be `NULL` only when `len` is zero.
- Example:

```c
const uint8_t bytes[] = {1, 2, 3};
k4c_buf_cursor cursor = k4c_buf_cursor_create(bytes, sizeof(bytes));
```

### k4c_buf_cursor_create_from_cstr

```c
k4c_buf_cursor k4c_buf_cursor_create_from_cstr(const char *text);
```

- Parameters: `text`
- Returns: cursor over the bytes before `text`'s terminating NUL.
- Example:

```c
k4c_buf_cursor cursor = k4c_buf_cursor_create_from_cstr("name = value");
```

### k4c_buf_cursor_at_end

```c
bool k4c_buf_cursor_at_end(const k4c_buf_cursor *cursor);
```

- Parameters: `cursor`
- Returns: `true` when no bytes remain.

### k4c_buf_cursor_position

```c
size_t k4c_buf_cursor_position(const k4c_buf_cursor *cursor);
```

- Parameters: `cursor`
- Returns: current zero-based offset from the start of the buffer.

### k4c_buf_cursor_remaining

```c
size_t k4c_buf_cursor_remaining(const k4c_buf_cursor *cursor);
```

- Parameters: `cursor`
- Returns: number of bytes from the current position to the end.

### k4c_buf_cursor_peek

```c
k4c_status k4c_buf_cursor_peek(const k4c_buf_cursor *cursor, uint8_t *out);
```

- Parameters: `cursor`, `out`
- Returns: `K4C_STATUS_OK` when a byte is available, or `K4C_STATUS_EOF`.
- Writes: current byte to `*out` on success.
- Notes: does not advance the cursor.

### k4c_buf_cursor_peek_at

```c
k4c_status k4c_buf_cursor_peek_at(const k4c_buf_cursor *cursor, size_t offset, uint8_t *out);
```

- Parameters: `cursor`, `offset`, `out`
- Returns: `K4C_STATUS_OK` when a byte is available, or `K4C_STATUS_EOF`.
- Writes: byte `offset` bytes after the current position to `*out` on success.
- Notes: does not advance the cursor.

### k4c_buf_cursor_starts_with

```c
bool k4c_buf_cursor_starts_with(const k4c_buf_cursor *cursor, const void *bytes, size_t len);
```

- Parameters: `cursor`, `bytes`, `len`
- Returns: whether the remaining buffer begins with `bytes[0..len)`.
- Notes: does not advance the cursor. `bytes` may be `NULL` only when `len` is zero.

### k4c_buf_cursor_next

```c
k4c_status k4c_buf_cursor_next(k4c_buf_cursor *cursor, uint8_t *out);
```

- Parameters: `cursor`, `out`
- Returns: `K4C_STATUS_OK` when a byte is consumed, or `K4C_STATUS_EOF`.
- Writes: consumed byte to `*out` on success.
- Notes: advances by one byte only on success.
- Example:

```c
uint8_t byte = 0;
while (k4c_buf_cursor_next(&cursor, &byte) == K4C_STATUS_OK) {
    /* use byte */
}
```

### k4c_buf_cursor_skip

```c
k4c_status k4c_buf_cursor_skip(k4c_buf_cursor *cursor, size_t count);
```

- Parameters: `cursor`, `count`
- Returns: `K4C_STATUS_OK` on success, or `K4C_STATUS_EOF` when fewer than `count`
  bytes remain.
- Notes: advances only when enough bytes remain.

### k4c_buf_cursor_next_if

```c
bool k4c_buf_cursor_next_if(k4c_buf_cursor *cursor, uint8_t expected);
```

- Parameters: `cursor`, `expected`
- Returns: `true` when the current byte matched and was consumed.
- Notes: advances by one byte only on match.

### k4c_buf_cursor_skip_while

```c
size_t k4c_buf_cursor_skip_while(
    k4c_buf_cursor *cursor,
    k4c_buf_cursor_predicate_fn predicate,
    void *context
);
```

- Parameters: `cursor`, `predicate`, `context`
- Returns: number of bytes skipped.
- Notes: calls `predicate(context, byte)` for each current byte and advances while
  it returns `true`.
- Example:

```c
static bool is_digit(void *context, uint8_t byte) {
    (void)context;
    return byte >= '0' && byte <= '9';
}

size_t digits = k4c_buf_cursor_skip_while(&cursor, is_digit, NULL);
```

### k4c_buf_cursor_skip_ascii_whitespace

```c
void k4c_buf_cursor_skip_ascii_whitespace(k4c_buf_cursor *cursor);
```

- Parameters: `cursor`
- Returns: none.
- Notes: advances past ASCII space, tab, newline, carriage return, form feed, and
  vertical tab.
