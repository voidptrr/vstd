# memory.bytes

## DESCRIPTION

The bytes module provides low-level byte-region helpers.

## FUNCTIONS

### ck_memswap

```c
void ck_memswap(void *a, void *b, size_t size);
```

- Parameters: `a`, `b`, `size`
- Behavior: swaps `size` bytes between regions `a` and `b`.
