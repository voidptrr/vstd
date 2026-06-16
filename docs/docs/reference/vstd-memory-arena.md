# memory.arena

## DESCRIPTION

The arena module provides a bump allocator backed by an internal contiguous
region. Allocation moves an offset forward, and `vs_arena_reset` releases all
arena allocations together by moving the offset back to zero.

Arena allocations are useful for short-lived groups of memory with the same
lifetime. Individual deallocation is unsupported, so the allocator adapter does
not advertise `VS_ALLOCATOR_FEATURE_DEALLOC` and does not provide a dealloc
callback.

This API is fail-fast for required initialization/teardown preconditions.

## TYPES

### vs_arena

```c
typedef struct vs_arena vs_arena;
```

- Notes: `vs_arena` is opaque. Use the functions below to inspect or mutate
  arena state.

## FUNCTIONS

### vs_arena_create

```c
vs_arena *vs_arena_create(size_t capacity);
```

- Parameters: `capacity`
- Returns: arena pointer.
- Notes: capacity is aligned up to the allocator's internal memory alignment.

### vs_arena_adapter

```c
vs_allocator vs_arena_adapter(vs_arena *arena);
```

- Parameters: `arena`
- Returns: allocator adapter bound to `arena`.
- Notes: the returned allocator advertises `VS_ALLOCATOR_FEATURE_REALLOC | VS_ALLOCATOR_FEATURE_RESET`.

### vs_arena_alloc

```c
void *vs_arena_alloc(vs_arena *arena, size_t size);
```

- Parameters: `arena`, `size`
- Returns: allocated pointer, or `NULL` when allocation cannot be satisfied.
- Notes: returned pointers are aligned to the allocator's internal memory alignment.

### vs_arena_realloc

```c
void *vs_arena_realloc(vs_arena *arena, void *ptr, size_t size);
```

- Parameters: `arena`, `ptr`, `size`
- Returns: grown pointer, original pointer when the aligned size is unchanged, or `NULL` on failure.
- Notes: `ptr == NULL` behaves like allocation. `size == 0` with an existing pointer returns `NULL`. Shrinking an existing allocation is invalid.

### vs_arena_reset

```c
void vs_arena_reset(vs_arena *arena);
```

- Parameters: `arena`
- Returns: none.
- Behavior: releases all arena allocations together while keeping the backing buffer.
- Notes: pointers returned before reset must not be used after reset.

### vs_arena_capacity

```c
size_t vs_arena_capacity(const vs_arena *arena);
```

- Parameters: `arena`
- Returns: total managed bytes.

### vs_arena_used

```c
size_t vs_arena_used(const vs_arena *arena);
```

- Parameters: `arena`
- Returns: number of bytes consumed from the arena buffer.

### vs_arena_available

```c
size_t vs_arena_available(const vs_arena *arena);
```

- Parameters: `arena`
- Returns: number of bytes still available.

### vs_arena_destroy

```c
void vs_arena_destroy(vs_arena *arena);
```

- Parameters: `arena`
- Returns: none.
- Behavior: releases the backing buffer and arena handle.

## EXAMPLE

```c
#include <stdint.h>

#include <vstd/memory/allocators/arena.h>

int main(void) {
    int status = 0;
    vs_arena *arena = vs_arena_create(1024);
    vs_allocator allocator = vs_arena_adapter(arena);

    uint64_t *value = vs_arena_alloc(arena, sizeof(uint64_t));
    if (value == NULL) {
        status = 1;
        goto cleanup;
    }

    *value = 42;

    vs_arena_reset(arena);

cleanup:
    vs_arena_destroy(arena);

    (void)allocator;
    return status;
}
```
