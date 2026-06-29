# io.reader

## DESCRIPTION

The reader module defines a small generic buffered byte-reader interface. It
does not own the underlying source or buffer. Concrete readers provide a context
pointer and a vtable read function; the generic reader owns the buffered window
state and implements byte and delimiter operations on top of that primitive.

This API is byte-oriented. Returned delimiter chunks are not NUL-terminated.

## TYPES

### k4c_reader

```c
typedef struct k4c_reader {
    void *ctx;
    const k4c_reader_vtable *vtable;
    uint8_t *data;
    size_t pos;
    size_t len;
    size_t capacity;
} k4c_reader;
```

- `ctx`: caller-owned concrete reader state.
- `vtable`: source operation table used when the buffer needs more bytes.
- `data`: caller-owned buffer used as the reader window.
- `pos`: current unread offset inside `data`.
- `len`: number of valid buffered bytes in `data`.
- `capacity`: total bytes available in `data`.

### k4c_reader_vtable

```c
typedef struct k4c_reader_vtable {
    k4c_reader_read_fn read;
} k4c_reader_vtable;
```

- `read`: implementation function for reading more bytes from the concrete
  source into a provided destination buffer.

## FUNCTIONS

### k4c_reader_create

```c
k4c_reader k4c_reader_create(
    void *ctx,
    const k4c_reader_vtable *vtable,
    uint8_t *data,
    size_t capacity
);
```

- Parameters: `ctx`, `vtable`, `data`, `capacity`
- Returns: a generic reader value.
- Notes: the reader does not take ownership of `ctx` or `data`. `vtable`,
  `vtable->read`, and `data` must not be `NULL`; `capacity` must be greater
  than zero.

### k4c_reader_take_byte

```c
k4c_status k4c_reader_take_byte(k4c_reader *reader, uint8_t *out);
```

- Parameters: `reader`, `out`
- Returns: `K4C_STATUS_OK` when one byte was consumed, `K4C_STATUS_EOF` when no
  byte is available, or another status from the concrete reader.
- Writes: the consumed byte to `*out` on success.
- Notes: refills the reader buffer through `vtable->read` when needed.

### k4c_reader_take_delimiter

```c
k4c_status k4c_reader_take_delimiter(
    k4c_reader *reader,
    uint8_t delimiter,
    k4c_buf_cursor *out
);
```

- Parameters: `reader`, `delimiter`, `out`
- Returns: `K4C_STATUS_OK` when bytes were consumed, `K4C_STATUS_EOF` when no
  bytes are available, `K4C_STATUS_OVERFLOW` when no delimiter fits in the
  reader buffer, or another status from the concrete reader.
- Writes: a cursor over the consumed bytes to `*out` on success.
- Notes: the returned cursor points into `reader->data` and is invalidated by
  the next reader operation that refills or rebases the buffer.

## EXAMPLES

### Implement a memory reader

Concrete readers keep only source-specific state. The vtable function copies
bytes from that source into the generic reader's buffer; byte and delimiter
operations are shared by all readers.

```c
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <k4c/buffer/cursor.h>
#include <k4c/error.h>
#include <k4c/io/reader.h>

typedef struct memory_reader {
    k4c_buf_cursor cursor;
} memory_reader;

static k4c_status memory_read(void *context, uint8_t *data, size_t capacity, size_t *out_len);

static const k4c_reader_vtable memory_reader_vtable = {
    .read = memory_read,
};

static k4c_status memory_read(void *context, uint8_t *data, size_t capacity, size_t *out_len) {
    memory_reader *reader = context;
    size_t remaining = k4c_buf_cursor_remaining(&reader->cursor);
    size_t read_len = remaining < capacity ? remaining : capacity;

    if (read_len > 0) {
        memcpy(data, reader->cursor.data + reader->cursor.pos, read_len);
        reader->cursor.pos += read_len;
        *out_len = read_len;
        return K4C_STATUS_OK;
    }

    *out_len = 0;
    return K4C_STATUS_EOF;
}

int main(void) {
    memory_reader memory = {
        .cursor = k4c_buf_cursor_create_from_cstr("first\nsecond"),
    };
    uint8_t buffer[128];
    k4c_reader reader = k4c_reader_create(
        &memory,
        &memory_reader_vtable,
        buffer,
        sizeof(buffer)
    );

    k4c_buf_cursor line;
    while (k4c_reader_take_delimiter(&reader, '\n', &line) == K4C_STATUS_OK) {
        /* line.data points into buffer and has line.len bytes */
    }
    return 0;
}
```

### Consume any reader

Functions can accept `k4c_reader *` when they only care about the byte stream,
not whether it comes from memory, a file reader, or another source.

```c
#include <stddef.h>
#include <stdint.h>

#include <k4c/error.h>
#include <k4c/io/reader.h>

k4c_status count_bytes(k4c_reader *reader, size_t *out) {
    uint8_t byte = 0;
    size_t count = 0;

    k4c_status st = K4C_STATUS_OK;
    while ((st = k4c_reader_take_byte(reader, &byte)) == K4C_STATUS_OK) {
        count += 1;
    }

    if (st != K4C_STATUS_EOF) {
        return st;
    }

    *out = count;
    return K4C_STATUS_OK;
}
```
