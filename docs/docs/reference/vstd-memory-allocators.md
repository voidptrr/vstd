# memory.allocator

## DESCRIPTION

The memory allocator API provides a generic allocator interface plus checked
allocation helpers used by `vstd` internals.

Owning APIs follow a construction-time allocator convention: pass an allocator
to `*_create`, and the created object stores that allocator pointer for later
growth and `*_destroy`. Mutation and destroy functions do not take another
allocator argument because freeing with a different allocator than the one used
for allocation is unsafe. The allocator object must outlive every object that
captures it. Passing `NULL` selects the C library heap.

## TYPES

### vs_allocator

```c
typedef struct vs_allocator {
    void *ctx;
    unsigned int features;
    vs_alloc_fn alloc;
    vs_realloc_fn realloc;
    vs_dealloc_fn dealloc;
} vs_allocator;
```

- Fields: `ctx`, `features`, `alloc`, `realloc`, `dealloc`
- Notes: owning containers store this pointer at create time and reuse it for
  later allocation, reallocation, and destroy work.

### vs_allocator_features

```c
typedef enum vs_allocator_features {
    VS_ALLOCATOR_FEATURE_DEALLOC = 1 << 0,
    VS_ALLOCATOR_FEATURE_REALLOC = 1 << 1,
    VS_ALLOCATOR_FEATURE_RESET = 1 << 2,
} vs_allocator_features;
```

- `VS_ALLOCATOR_FEATURE_DEALLOC`: allocator supports freeing individual allocations.
- `VS_ALLOCATOR_FEATURE_REALLOC`: allocator supports resizing allocations.
- `VS_ALLOCATOR_FEATURE_RESET`: allocator supports releasing all allocations together.

## FUNCTIONS

### vs_alloc

```c
vs_status vs_alloc(vs_allocator *allocator, size_t size, void **out);
```

- Parameters: `allocator`, `size`, `out`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Writes: allocated memory pointer to `*out` on success; `NULL` on failure.
- Notes: when `allocator` is `NULL` or has no `alloc` callback, uses the C library heap.

### vs_resize

```c
vs_status vs_resize(vs_allocator *allocator, void *ptr, size_t size, void **out);
```

- Parameters: `allocator`, `ptr`, `size`, `out`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Writes: resized memory pointer to `*out` on success; `NULL` when `size == 0` or allocation fails.
- Notes: when `allocator` is `NULL` or has no `realloc` callback, uses the C library heap. When `size == 0`, deallocates `ptr`.

### vs_dealloc

```c
void vs_dealloc(vs_allocator *allocator, void *ptr);
```

- Parameters: `allocator`, `ptr`
- Returns: none.
- Notes: when `allocator` is `NULL`, uses the C library heap. If a custom allocator does not advertise `VS_ALLOCATOR_FEATURE_DEALLOC`, this function returns without calling a dealloc callback.
