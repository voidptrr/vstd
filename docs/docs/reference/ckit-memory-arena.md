# memory.arena

## DESCRIPTION

The arena module provides a bump allocator backed by an internal contiguous
region. Allocation moves an offset forward, and `ck_arena_reset` releases all
arena allocations together by moving the offset back to zero.

Arena allocations are useful for short-lived groups of memory with the same
lifetime. Individual deallocation is unsupported, so the allocator adapter does
not advertise `CK_ALLOCATOR_FEATURE_DEALLOC` and does not provide a dealloc
callback.

This API is fail-fast for required initialization/teardown preconditions.

## TYPES

### ck_arena

```c
typedef struct ck_arena ck_arena;
```

- Notes: `ck_arena` is opaque. Use the functions below to inspect or mutate
  arena state.

## FUNCTIONS

### ck_arena_init

```c
ck_arena *ck_arena_init(size_t capacity);
```

- Parameters: `capacity`
- Returns: arena pointer.
- Notes: capacity is aligned up to the allocator's internal memory alignment.

### ck_arena_allocator

```c
ck_allocator ck_arena_allocator(ck_arena *arena);
```

- Parameters: `arena`
- Returns: allocator adapter bound to `arena`.
- Notes: the returned allocator advertises `CK_ALLOCATOR_FEATURE_REALLOC | CK_ALLOCATOR_FEATURE_RESET`.

### ck_arena_alloc

```c
void *ck_arena_alloc(ck_arena *arena, size_t size);
```

- Parameters: `arena`, `size`
- Returns: allocated pointer, or `NULL` when allocation cannot be satisfied.
- Notes: returned pointers are aligned to the allocator's internal memory alignment.

### ck_arena_realloc

```c
void *ck_arena_realloc(ck_arena *arena, void *ptr, size_t size);
```

- Parameters: `arena`, `ptr`, `size`
- Returns: grown pointer, original pointer when the aligned size is unchanged, or `NULL` on failure.
- Notes: `ptr == NULL` behaves like allocation. `size == 0` with an existing pointer returns `NULL`. Shrinking an existing allocation is invalid.

### ck_arena_reset

```c
void ck_arena_reset(ck_arena *arena);
```

- Parameters: `arena`
- Returns: none.
- Behavior: releases all arena allocations together while keeping the backing buffer.
- Notes: pointers returned before reset must not be used after reset.

### ck_arena_capacity

```c
size_t ck_arena_capacity(const ck_arena *arena);
```

- Parameters: `arena`
- Returns: total managed bytes.

### ck_arena_used

```c
size_t ck_arena_used(const ck_arena *arena);
```

- Parameters: `arena`
- Returns: number of bytes consumed from the arena buffer.

### ck_arena_available

```c
size_t ck_arena_available(const ck_arena *arena);
```

- Parameters: `arena`
- Returns: number of bytes still available.

### ck_arena_deinit

```c
void ck_arena_deinit(ck_arena *arena);
```

- Parameters: `arena`
- Returns: none.
- Behavior: releases the backing buffer and arena handle.

## EXAMPLE

```c
#include <stdint.h>

#include <ckit/memory/allocators/arena.h>

int main(void) {
    int status = 0;
    ck_arena *arena = ck_arena_init(1024);
    ck_allocator allocator = ck_arena_allocator(arena);

    uint64_t *value = ck_arena_alloc(arena, sizeof(uint64_t));
    if (value == NULL) {
        status = 1;
        goto cleanup;
    }

    *value = 42;

    ck_arena_reset(arena);

cleanup:
    ck_arena_deinit(arena);

    (void)allocator;
    return status;
}
```
