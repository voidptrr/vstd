# datastruct.iterator

## DESCRIPTION

The iterator module provides a generic iterator that can walk all vstd data
structures with iterator support. Data structures own their iterator state,
construction, and advancement functions. The generic iterator module only owns
the basic callback iterator shape and eager collection helpers.

Iterators do not allocate. They borrow the data structure or caller-owned
callback context they point at.

The `context` parameter is intentionally generic. For `vs_iterator_from_callback`
it is usually cursor state, such as an index into a vector. Pass `NULL` when a
callback does not need extra data.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_iterator

```c
typedef struct vs_iterator vs_iterator;
```

Iterator value. Store it by value and pass its address to `vs_iterator_next`.

### vs_iterator_next_fn

```c
typedef const void *(*vs_iterator_next_fn)(void *context);
```

Callback used by custom iterators. Return the next item pointer, or `NULL` when
the iterator is exhausted.

### vs_iterator_map_into_fn

```c
typedef void (*vs_iterator_map_into_fn)(void *context, const void *src, void *dst);
```

Map callback used by `collect_map`. Write the mapped value into `dst`. The
destination storage has the element size passed to `vs_iterator_collect_map`.

## FUNCTIONS

### VS_ITER_NEXT_AS

```c
#define VS_ITER_NEXT_AS(type, iter)
```

- Parameters: `type`, `iter`
- Returns: typed pointer from `vs_iterator_next`.
- Example:

```c
const int *item = VS_ITER_NEXT_AS(int, &iter);
```

### VS_ITERATOR_FOR_EACH

```c
#define VS_ITERATOR_FOR_EACH(type, item, iter)
```

- Parameters: `type`, `item`, `iter`
- Notes: declares `const type *item` scoped to the loop body.
- Example:

```c
VS_ITERATOR_FOR_EACH(int, item, &iter) {
    /* use *item */
}
```

### vs_iterator_from_callback

```c
vs_iterator vs_iterator_from_callback(void *context, vs_iterator_next_fn next);
```

- Parameters: `context`, `next`
- Returns: iterator backed by caller-owned state.
- Notes: use this to integrate custom data structures with vstd helpers such as
  `vs_iterator_collect`.
- Example:

```c
typedef struct range_state {
    int current;
    int end;
} range_state;

static const void *range_next(void *context) {
    range_state *state = context;
    if (state->current >= state->end) {
        return NULL;
    }
    state->current += 1;
    return &state->current;
}

range_state state = {.current = 0, .end = 10};
vs_iterator iter = vs_iterator_from_callback(&state, range_next);
```

### vs_iterator_next

```c
const void *vs_iterator_next(vs_iterator *iter);
```

- Parameters: `iter`
- Returns: next item pointer, or `NULL` when exhausted.
- Example:

```c
const int *item;
while ((item = (const int *)vs_iterator_next(&iter)) != NULL) {
    /* use *item */
}
```

### vs_iterator_set_size_hint

```c
void vs_iterator_set_size_hint(vs_iterator *iter, size_t size_hint);
```

- Parameters: `iter`, `size_hint`
- Returns: none.
- Notes: sets a conservative remaining-item count used by collect helpers to
  reserve output storage.

### vs_iterator_size_hint

```c
size_t vs_iterator_size_hint(const vs_iterator *iter);
```

- Parameters: `iter`
- Returns: remaining-item hint, or zero when unknown.

### vs_iterator_collect

```c
vs_vector *vs_iterator_collect(vs_iterator *source,
                               size_t elem_size,
                               vs_allocator *allocator);
```

- Parameters: `source`, `elem_size`, `allocator`
- Returns: vector containing copies of the remaining source items.
- Notes: consumes `source`. The returned vector owns its storage and can outlive
  the original data structure.
- Example:

```c
vs_vector *values = vs_vector_create(sizeof(int), NULL);
int one = 1;
int two = 2;
vs_vector_push(values, &one);
vs_vector_push(values, &two);

vs_iterator iter = vs_vector_get_iterator(values);
vs_vector *copy = vs_iterator_collect(&iter, sizeof(int), NULL);

vs_vector_destroy(values);
/* copy still owns the collected ints. */
vs_vector_destroy(copy);
```

### vs_iterator_collect_map

```c
vs_vector *vs_iterator_collect_map(vs_iterator *source,
                                   size_t dst_elem_size,
                                   vs_iterator_map_into_fn map,
                                   void *context,
                                   vs_allocator *allocator);
```

- Parameters: `source`, `dst_elem_size`, `map`, `context`, `allocator`
- Returns: vector containing mapped copies of the remaining source items.
- Notes: consumes `source`. Use this when the output element type or size is
  different from the source element type.
- Example:

```c
static void int_to_double(void *context, const void *src, void *dst) {
    (void)context;
    *(double *)dst = (double)*(const int *)src;
}

vs_vector *ints = vs_vector_create(sizeof(int), NULL);
int value = 42;
vs_vector_push(ints, &value);

vs_iterator iter = vs_vector_get_iterator(ints);
vs_vector *doubles =
    vs_iterator_collect_map(&iter, sizeof(double), int_to_double, NULL, NULL);

const double *mapped = (const double *)vs_vector_get_const(doubles, 0);
/* *mapped == 42.0 */

vs_vector_destroy(ints);
vs_vector_destroy(doubles);
```
