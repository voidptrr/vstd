# cstd.mem.allocator

## DESCRIPTION

The allocator module provides fail-fast allocation helpers used by `cstd` internals.

## FUNCTIONS

### cstd_malloc

```c
void *cstd_malloc(size_t size);
```

- Parameters: `size`
- Returns: pointer to allocated memory.
- Behavior: prints a fatal message and aborts on out-of-memory.

### cstd_realloc

```c
void *cstd_realloc(void *ptr, size_t size);
```

- Parameters: `ptr`, `size`
- Returns: pointer to resized memory.
- Behavior: prints a fatal message and aborts on allocation failure.
- Notes: when `size == 0`, behavior follows C library `realloc` semantics.

## SEE ALSO

`cstd.mem`, `cstd.mem.bytes`
