# memory.arena

## DESCRIPTION

The arena module provides a bump allocator backed by an internal contiguous
region. Allocation moves an offset forward, and `ckit_arena_reset` releases all
arena allocations together by moving the offset back to zero.

Arena allocations are useful for short-lived groups of memory with the same
lifetime. Individual deallocation is unsupported, so the allocator adapter does
not advertise `CKIT_ALLOCATOR_FEATURE_DEALLOC` and does not provide a dealloc
callback.

This API is fail-fast for required initialization/teardown preconditions.

## MEMORY MODEL

```text
+----------------------+----------------------+
| used bytes           | available bytes      |
+----------------------+----------------------+
                       ^
                       arena->offset
```

Each allocation stores a small internal header before the returned pointer so
`ckit_arena_realloc` can preserve existing bytes when growing an allocation.

## TYPES

### ckit_arena

```c
typedef struct ckit_arena {
    void *buffer;
    size_t capacity;
    size_t offset;
} ckit_arena;
```

- Fields: `buffer`, `capacity`, `offset`
- Notes: treat these fields as implementation details outside low-level allocator code.

## FUNCTIONS

### ckit_arena_init

```c
ckit_allocator ckit_arena_init(ckit_arena *arena, size_t capacity);
```

- Parameters: `arena`, `capacity`
- Returns: allocator adapter bound to `arena`.
- Notes: capacity is aligned up to `CKIT_MEMORY_ALIGN`. The returned allocator advertises `CKIT_ALLOCATOR_FEATURE_REALLOC | CKIT_ALLOCATOR_FEATURE_RESET`.

### ckit_arena_free

```c
void ckit_arena_free(ckit_arena *arena);
```

- Parameters: `arena`
- Returns: none.
- Behavior: releases the backing buffer and clears arena state.

### ckit_arena_alloc

```c
void *ckit_arena_alloc(ckit_arena *arena, size_t size);
```

- Parameters: `arena`, `size`
- Returns: allocated pointer, or `NULL` when allocation cannot be satisfied.
- Notes: returned pointers are aligned to `CKIT_MEMORY_ALIGN`.

### ckit_arena_realloc

```c
void *ckit_arena_realloc(ckit_arena *arena, void *ptr, size_t size);
```

- Parameters: `arena`, `ptr`, `size`
- Returns: grown pointer, original pointer when the aligned size is unchanged, or `NULL` on failure.
- Notes: `ptr == NULL` behaves like allocation. `size == 0` returns `NULL`. Shrinking an existing allocation is invalid.

### ckit_arena_reset

```c
void ckit_arena_reset(ckit_arena *arena);
```

- Parameters: `arena`
- Returns: none.
- Behavior: releases all arena allocations together while keeping the backing buffer.
- Notes: pointers returned before reset must not be used after reset.

### ckit_arena_capacity

```c
size_t ckit_arena_capacity(const ckit_arena *arena);
```

- Parameters: `arena`
- Returns: total managed bytes.

### ckit_arena_used

```c
size_t ckit_arena_used(const ckit_arena *arena);
```

- Parameters: `arena`
- Returns: number of bytes consumed from the arena buffer.

### ckit_arena_available

```c
size_t ckit_arena_available(const ckit_arena *arena);
```

- Parameters: `arena`
- Returns: number of bytes still available.

## EXAMPLE

```c
#include <stdint.h>

#include <ckit/memory/allocators/arena.h>

int main(void) {
    ckit_arena arena;
    ckit_allocator allocator = ckit_arena_init(&arena, 1024);

    uint64_t *value = ckit_arena_alloc(&arena, sizeof(*value));
    if (value == NULL) {
        ckit_arena_free(&arena);
        return 1;
    }

    *value = 42;

    ckit_arena_reset(&arena);
    ckit_arena_free(&arena);

    (void)allocator;
    return 0;
}
```
