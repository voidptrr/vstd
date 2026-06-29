# io.file_reader

## DESCRIPTION

The file reader module provides a fixed-buffer reader over an existing `FILE *`.
It is useful when a program wants to walk file contents incrementally without
loading the whole file into memory.

The reader does not own the file stream or the backing buffer. The caller opens
and closes the `FILE *`, provides the byte storage, and keeps both alive for as
long as the reader is used.

`k4c_file_reader_create` initializes the file reader. Use
`k4c_file_reader_view` to create a generic `k4c_reader` value over it, then
use `k4c_reader_take_byte` to consume one byte, or
`k4c_reader_take_delimiter` to consume through a delimiter.

This API is byte-oriented. Returned chunks are not NUL-terminated.

## TYPES

### k4c_file_reader

```c
typedef struct k4c_file_reader {
    FILE *file;
    uint8_t *data;
    size_t buffer_capacity;
} k4c_file_reader;
```

- `file`: caller-owned `FILE *`.
- `data`: caller-owned byte buffer used by the generic reader view.
- `buffer_capacity`: maximum bytes held in `data` for the buffered window.

## FUNCTIONS

### k4c_file_reader_create

```c
k4c_status k4c_file_reader_create(
    k4c_file_reader *reader,
    FILE *file,
    uint8_t *data,
    size_t buffer_capacity
);
```

- Parameters: `reader`, `file`, `data`, `buffer_capacity`
- Returns: `K4C_STATUS_OK` on success, or `K4C_STATUS_INVALID_ARGUMENT` when
  `file` is `NULL`, `data` is `NULL`, or `buffer_capacity` is zero.
- Notes: the reader does not take ownership of `file` or `data`.

### k4c_file_reader_view

```c
k4c_reader k4c_file_reader_view(k4c_file_reader *reader);
```

- Parameters: `reader`
- Returns: generic reader value over `reader`.
- Notes: the returned value is non-owning and stores its current buffered state
  by value. `reader` and its buffer must outlive it.

### k4c_file_reader_close

```c
k4c_status k4c_file_reader_close(k4c_file_reader *reader);
```

- Parameters: `reader`
- Returns: `K4C_STATUS_OK`.
- Notes: resets the reader. It does not call `fclose` and does not release the
  caller-provided buffer.

## EXAMPLES

### Read bytes

```c
#include <stdint.h>
#include <stdio.h>

#include <k4c/error.h>
#include <k4c/io/file_reader.h>
#include <k4c/io/reader.h>

int main(void) {
    FILE *file = fopen("data.bin", "rb");
    if (file == NULL) {
        return 1;
    }

    uint8_t buffer[4096];
    k4c_file_reader file_reader;
    if (k4c_file_reader_create(&file_reader, file, buffer, sizeof(buffer)) != K4C_STATUS_OK) {
        fclose(file);
        return 1;
    }
    k4c_reader reader = k4c_file_reader_view(&file_reader);

    uint8_t byte = 0;
    k4c_status st = K4C_STATUS_OK;
    while ((st = k4c_reader_take_byte(&reader, &byte)) == K4C_STATUS_OK) {
        fwrite(&byte, 1, 1, stdout);
    }

    k4c_file_reader_close(&file_reader);
    fclose(file);

    if (st != K4C_STATUS_EOF) {
        return 1;
    }

    return 0;
}
```

### Read lines

```c
#include <stdint.h>
#include <stdio.h>

#include <k4c/buffer/cursor.h>
#include <k4c/error.h>
#include <k4c/io/file_reader.h>
#include <k4c/io/reader.h>

int main(void) {
    FILE *file = fopen("access.log", "rb");
    if (file == NULL) {
        return 1;
    }

    uint8_t buffer[64 * 1024];
    k4c_file_reader file_reader;
    if (k4c_file_reader_create(&file_reader, file, buffer, sizeof(buffer)) != K4C_STATUS_OK) {
        fclose(file);
        return 1;
    }
    k4c_reader reader = k4c_file_reader_view(&file_reader);

    k4c_buf_cursor line;
    k4c_status st = K4C_STATUS_OK;
    while ((st = k4c_reader_take_delimiter(&reader, '\n', &line)) == K4C_STATUS_OK) {
        fwrite(line.data, 1, line.len, stdout);
    }

    k4c_file_reader_close(&file_reader);
    fclose(file);

    if (st == K4C_STATUS_OVERFLOW) {
        return 1;
    } else if (st != K4C_STATUS_EOF) {
        return 1;
    }

    return 0;
}
```

Use `fwrite` or another length-aware API for output. `line.data` is not
NUL-terminated, so it must not be printed with `%s`.
