# memory.arena

## DESCRIPTION

The arena module provides a bump arena backed by an internal contiguous
region. Allocation moves an offset forward, and `arena_reset` releases all
arena allocations together by moving the offset back to zero.

Arena allocations are useful for short-lived groups of memory with the same
lifetime. Individual deallocation is unsupported, so the generic allocator view
does not advertise `ALLOCATOR_FEATURE_DEALLOC` and does not provide a dealloc
callback.

This API is fail-fast for required initialization/teardown preconditions.

## TYPES

### arena

```c
typedef struct arena arena;
```

- Notes: `arena` is opaque. Use the functions below to inspect or mutate
  arena state.

## FUNCTIONS

### arena_create

```c
status arena_create(size_t capacity, arena **out);
```

- Parameters: `capacity`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: arena pointer to `*out` on success.
- Notes: capacity is aligned up to the arena's internal memory alignment.

### arena_allocator

```c
allocator *arena_allocator(arena *arena);
```

- Parameters: `arena`
- Returns: generic allocator view owned by `arena`.
- Notes: the returned allocator advertises `ALLOCATOR_FEATURE_REALLOC | ALLOCATOR_FEATURE_RESET`.

### arena_alloc

```c
void *arena_alloc(arena *arena, size_t size);
```

- Parameters: `arena`, `size`
- Returns: allocated pointer, or `NULL` when allocation cannot be satisfied.
- Notes: returned pointers are aligned to the arena's internal memory alignment.

### arena_realloc

```c
void *arena_realloc(arena *arena, void *ptr, size_t size);
```

- Parameters: `arena`, `ptr`, `size`
- Returns: grown pointer, original pointer when the aligned size is unchanged, or `NULL` on failure.
- Notes: `ptr == NULL` behaves like allocation. `size == 0` with an existing pointer returns `NULL`. Shrinking an existing allocation is invalid.

### arena_reset

```c
void arena_reset(arena *arena);
```

- Parameters: `arena`
- Returns: none.
- Behavior: releases all arena allocations together while keeping the backing buffer.
- Notes: pointers returned before reset must not be used after reset.

### arena_capacity

```c
size_t arena_capacity(const arena *arena);
```

- Parameters: `arena`
- Returns: total managed bytes.

### arena_used

```c
size_t arena_used(const arena *arena);
```

- Parameters: `arena`
- Returns: number of bytes consumed from the arena buffer.

### arena_available

```c
size_t arena_available(const arena *arena);
```

- Parameters: `arena`
- Returns: number of bytes still available.

### arena_destroy

```c
void arena_destroy(arena *arena);
```

- Parameters: `arena`
- Returns: none.
- Behavior: releases the backing buffer and arena handle.

## EXAMPLE

```c
#include <stdint.h>

#include <vstd/memory/arena.h>

int main(void) {
    int status = 0;
    arena *arena = NULL;
if (arena_create(1024, &arena) != STATUS_OK) {
    /* handle allocation failure */
}
    uint64_t *value = arena_alloc(arena, sizeof(uint64_t));
    if (value == NULL) {
        status = 1;
        goto cleanup;
    }

    *value = 42;

    arena_reset(arena);

cleanup:
    arena_destroy(arena);

    return status;
}
```
