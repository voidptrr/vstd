# io.file_reader

## DESCRIPTION

The file reader module provides a small fixed-buffer reader over an existing
`FILE *`. It is useful when a program wants to walk file contents incrementally
without loading the whole file into memory.

The reader does not own the file stream or the backing buffer. The caller opens
and closes the `FILE *`, provides the byte storage, and keeps both alive for as
long as the reader is used.

`k4c_file_reader_next` returns a `k4c_buf_cursor` over the current chunk. That
cursor points into the caller-provided buffer and remains valid until the next
reader operation that rewrites the buffer.

This API is byte-oriented. Returned chunks are not NUL-terminated, even in line
mode.

## TYPES

### k4c_file_reader_mode

```c
typedef enum k4c_file_reader_mode {
    K4C_FILE_READER_MODE_BYTES,
    K4C_FILE_READER_MODE_LINE,
} k4c_file_reader_mode;
```

- `K4C_FILE_READER_MODE_BYTES`: read up to `buffer_capacity` bytes per call.
- `K4C_FILE_READER_MODE_LINE`: read through the next newline when it fits in
  the buffer.

### k4c_file_reader

```c
typedef struct k4c_file_reader {
    k4c_file_reader_mode mode;
    FILE *file;
    uint8_t *data;
    size_t len;
    size_t buffer_capacity;
} k4c_file_reader;
```

- `file`: caller-owned `FILE *`.
- `data`: caller-owned byte buffer.
- `len`: number of valid bytes currently in `data`.
- `buffer_capacity`: maximum bytes held in `data` for one result.

## FUNCTIONS

### k4c_file_reader_init

```c
k4c_status k4c_file_reader_init(
    k4c_file_reader *reader,
    FILE *file,
    k4c_file_reader_mode mode,
    uint8_t *data,
    size_t buffer_capacity
);
```

- Parameters: `reader`, `file`, `mode`, `data`, `buffer_capacity`
- Returns: `K4C_STATUS_OK` on success, or `K4C_STATUS_INVALID_ARGUMENT` when
  the mode is invalid, `file` is `NULL`, `data` is `NULL`, or
  `buffer_capacity` is zero.
- Notes: the reader does not take ownership of `file` or `data`.

### k4c_file_reader_next

```c
k4c_status k4c_file_reader_next(k4c_file_reader *reader, k4c_buf_cursor *out);
```

- Parameters: `reader`, `out`
- Returns: `K4C_STATUS_OK` when a chunk was read, `K4C_STATUS_EOF` when no more
  data is available, `K4C_STATUS_OVERFLOW` when a line does not fit in line
  mode, or `K4C_STATUS_IO` for stream errors.
- Writes: a cursor over the current chunk to `*out` when returning
  `K4C_STATUS_OK`.
- Notes: line chunks include the terminating newline when present. Lines that
  fill the buffer without a newline return `K4C_STATUS_OVERFLOW`.

### k4c_file_reader_close

```c
k4c_status k4c_file_reader_close(k4c_file_reader *reader);
```

- Parameters: `reader`
- Returns: `K4C_STATUS_OK`.
- Notes: resets the reader. It does not call `fclose` and does not release the
  caller-provided buffer.

## EXAMPLES

### Read byte chunks

```c
FILE *file = fopen("data.bin", "rb");
if (file == NULL) {
    /* handle error */
}

uint8_t buffer[4096];
k4c_file_reader reader;
if (k4c_file_reader_init(
        &reader,
        file,
        K4C_FILE_READER_MODE_BYTES,
        buffer,
        sizeof(buffer)
    )
    != K4C_STATUS_OK) {
    fclose(file);
    /* handle error */
}

k4c_buf_cursor chunk;
k4c_status st = K4C_STATUS_OK;
while ((st = k4c_file_reader_next(&reader, &chunk)) == K4C_STATUS_OK) {
    fwrite(chunk.data, 1, chunk.len, stdout);
}

k4c_file_reader_close(&reader);
fclose(file);

if (st != K4C_STATUS_EOF) {
    /* handle read error */
}
```

### Read lines

```c
FILE *file = fopen("access.log", "rb");
if (file == NULL) {
    /* handle error */
}

uint8_t buffer[64 * 1024];
k4c_file_reader reader;
if (k4c_file_reader_init(
        &reader,
        file,
        K4C_FILE_READER_MODE_LINE,
        buffer,
        sizeof(buffer)
    )
    != K4C_STATUS_OK) {
    fclose(file);
    /* handle error */
}

k4c_buf_cursor line;
k4c_status st = K4C_STATUS_OK;
while ((st = k4c_file_reader_next(&reader, &line)) == K4C_STATUS_OK) {
    fwrite(line.data, 1, line.len, stdout);
}

k4c_file_reader_close(&reader);
fclose(file);

if (st == K4C_STATUS_OVERFLOW) {
    /* a line was longer than the configured buffer */
} else if (st != K4C_STATUS_EOF) {
    /* handle read error */
}
```

Use `fwrite` or another length-aware API for output. `line.data` is not
NUL-terminated, so it must not be printed with `%s`.
