# memory.allocators

## DESCRIPTION

The allocators module provides a generic allocator interface plus fail-fast
allocation helpers used by `ckit` internals.

## TYPES

### ckit_allocator

```c
typedef struct ckit_allocator {
    void *ctx;
    ckit_alloc_fn alloc;
    ckit_realloc_fn realloc;
    ckit_dealloc_fn dealloc;
} ckit_allocator;
```

- Fields: `ctx`, `alloc`, `realloc`, `dealloc`
- Notes: containers can use these callbacks when a custom allocator is provided.

## FUNCTIONS

### ckit_malloc

```c
void *ckit_malloc(ckit_allocator *allocator, size_t size);
```

- Parameters: `allocator`, `size`
- Returns: pointer to allocated memory.
- Behavior: prints a fatal message and aborts on out-of-memory.
- Notes: when `allocator` is `NULL`, uses the C library heap.

### ckit_realloc

```c
void *ckit_realloc(ckit_allocator *allocator, void *ptr, size_t size);
```

- Parameters: `allocator`, `ptr`, `size`
- Returns: pointer to resized memory.
- Behavior: prints a fatal message and aborts on allocation failure.
- Notes: when `allocator` is `NULL`, uses the C library heap. When `size == 0`, frees `ptr` and returns `NULL`.

### ckit_dealloc

```c
void ckit_dealloc(ckit_allocator *allocator, void *ptr);
```

- Parameters: `allocator`, `ptr`
- Returns: none.
- Notes: when `allocator` is `NULL`, uses the C library heap. `NULL` pointers are ignored.
