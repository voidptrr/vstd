# memory.allocators

## DESCRIPTION

The allocators module provides a generic allocator interface plus fail-fast
allocation helpers used by `ckit` internals.

## TYPES

### ck_allocator

```c
typedef struct ck_allocator {
    void *ctx;
    unsigned int features;
    ck_alloc_fn alloc;
    ck_realloc_fn realloc;
    ck_dealloc_fn dealloc;
} ck_allocator;
```

- Fields: `ctx`, `features`, `alloc`, `realloc`, `dealloc`
- Notes: containers can use these callbacks when a custom allocator is provided.

### ck_allocator_features

```c
typedef enum ck_allocator_features {
    CK_ALLOCATOR_FEATURE_DEALLOC = 1 << 0,
    CK_ALLOCATOR_FEATURE_REALLOC = 1 << 1,
    CK_ALLOCATOR_FEATURE_RESET = 1 << 2,
} ck_allocator_features;
```

- `CK_ALLOCATOR_FEATURE_DEALLOC`: allocator supports freeing individual allocations.
- `CK_ALLOCATOR_FEATURE_REALLOC`: allocator supports resizing allocations.
- `CK_ALLOCATOR_FEATURE_RESET`: allocator supports releasing all allocations together.

## FUNCTIONS

### ck_malloc

```c
void *ck_malloc(ck_allocator *allocator, size_t size);
```

- Parameters: `allocator`, `size`
- Returns: pointer to allocated memory.
- Behavior: prints a fatal message and aborts on out-of-memory.
- Notes: when `allocator` is `NULL` or has no `alloc` callback, uses the C library heap.

### ck_realloc

```c
void *ck_realloc(ck_allocator *allocator, void *ptr, size_t size);
```

- Parameters: `allocator`, `ptr`, `size`
- Returns: pointer to resized memory.
- Behavior: prints a fatal message and aborts on allocation failure.
- Notes: when `allocator` is `NULL` or has no `realloc` callback, uses the C library heap. When `size == 0`, deallocates `ptr` and returns `NULL`.

### ck_dealloc

```c
void ck_dealloc(ck_allocator *allocator, void *ptr);
```

- Parameters: `allocator`, `ptr`
- Returns: none.
- Notes: when `allocator` is `NULL`, uses the C library heap. If a custom allocator does not advertise `CK_ALLOCATOR_FEATURE_DEALLOC`, this function returns without calling a dealloc callback.
