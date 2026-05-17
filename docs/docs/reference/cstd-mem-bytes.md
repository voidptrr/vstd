# cstd.mem.bytes

## DESCRIPTION

The bytes module provides low-level byte-region helpers.

## FUNCTIONS

### cstd_memswap

```c
void cstd_memswap(void *a, void *b, size_t size);
```

- Parameters: `a`, `b`, `size`
- Behavior: swaps `size` bytes between regions `a` and `b`.

## SEE ALSO

`cstd.mem`, `cstd.mem.allocator`
