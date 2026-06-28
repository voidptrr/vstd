# io.file

## DESCRIPTION

The file module provides small, concrete filesystem helpers. These functions
operate on paths and return `k4c_status` values instead of exposing platform
error codes.

`k4c_file_read_all` loads the entire file into memory. It is intended for
configuration files, parser input, fixtures, and other bounded files.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### k4c_file_is_dir

```c
k4c_status k4c_file_is_dir(const char *path, bool *out);
```

- Parameters: `path`, `out`
- Returns: `K4C_STATUS_OK` on success, `K4C_STATUS_NOT_FOUND` when `path` does
  not exist, or `K4C_STATUS_IO` for other filesystem errors.
- Writes: whether `path` exists and is a directory to `*out`.

### k4c_file_size

```c
k4c_status k4c_file_size(const char *path, size_t *out);
```

- Parameters: `path`, `out`
- Returns: `K4C_STATUS_OK` on success, `K4C_STATUS_NOT_FOUND` when `path` does
  not exist, `K4C_STATUS_INVALID_ARGUMENT` when `path` is not a regular file,
  `K4C_STATUS_OVERFLOW` when the file size cannot fit in `size_t`, or
  `K4C_STATUS_IO` for other filesystem errors.
- Writes: regular file size in bytes to `*out`.

### k4c_file_read_all

```c
k4c_status k4c_file_read_all(
    const char *path,
    k4c_allocator *k4c_allocator,
    uint8_t **out_data,
    size_t *out_len
);
```

- Parameters: `path`, `k4c_allocator`, `out_data`, `out_len`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: allocator-owned file contents to `*out_data` and byte length to
  `*out_len`.
- Notes: allocates `*out_len + 1` bytes and writes a trailing NUL byte for
  convenience. The returned length excludes that terminator. Release `*out_data`
  with `k4c_dealloc` and the same allocator.
- Example:

```c
uint8_t *data = NULL;
size_t len = 0;
if (k4c_file_read_all("config.json", NULL, &data, &len) != K4C_STATUS_OK) {
    /* handle error */
}

k4c_buf_cursor cursor = k4c_buf_cursor_create(data, len);

k4c_dealloc(NULL, data);
```

### k4c_file_write_all

```c
k4c_status k4c_file_write_all(const char *path, const void *data, size_t len);
```

- Parameters: `path`, `data`, `len`
- Returns: `K4C_STATUS_OK` on success, or `K4C_STATUS_IO`.
- Notes: writes `data[0..len)` to `path`, replacing any existing file.
