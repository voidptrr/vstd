# ds.iterator

## DESCRIPTION

The iterator module provides a generic iterator that can walk all vstd data
structures with iterator support. Data structures own their iterator state,
construction, and advancement functions. The generic iterator module only owns
the basic callback iterator shape and eager collection helpers.

Iterators do not allocate. They borrow the data structure or caller-owned
callback context they point at.

The `context` parameter is intentionally generic. For `iterator_from_callback`
it is usually cursor state, such as an index into a vector. Pass `NULL` when a
callback does not need extra data.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### iterator

```c
typedef struct iterator iterator;
```

Iterator value. Store it by value and pass its address to `iterator_next`.

### iterator_next_fn

```c
typedef const void *(*iterator_next_fn)(void *context);
```

Callback used by custom iterators. Return the next item pointer, or `NULL` when
the iterator is exhausted.

### iterator_map_into_fn

```c
typedef void (*iterator_map_into_fn)(void *context, const void *src, void *dst);
```

Map callback used by `collect_map`. Write the mapped value into `dst`. The
destination storage has the element size passed to `iterator_collect_map`.

## FUNCTIONS

### ITER_NEXT_AS

```c
#define ITER_NEXT_AS(type, iter)
```

- Parameters: `type`, `iter`
- Returns: typed pointer from `iterator_next`.
- Example:

```c
const int *item = ITER_NEXT_AS(int, &iter);
```

### iterator_for_each

```c
#define iterator_for_each(type, item, iter)
```

- Parameters: `type`, `item`, `iter`
- Notes: declares `const type *item` scoped to the loop body.
- Example:

```c
iterator_for_each(int, item, &iter) {
    /* use *item */
}
```

### iterator_from_callback

```c
iterator iterator_from_callback(void *context, iterator_next_fn next);
```

- Parameters: `context`, `next`
- Returns: iterator backed by caller-owned state.
- Notes: use this to integrate custom data structures with vstd helpers such as
  `iterator_collect`.
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
iterator iter = iterator_from_callback(&state, range_next);
```

### iterator_next

```c
const void *iterator_next(iterator *iter);
```

- Parameters: `iter`
- Returns: next item pointer, or `NULL` when exhausted.
- Example:

```c
const int *item;
while ((item = (const int *)iterator_next(&iter)) != NULL) {
    /* use *item */
}
```

### iterator_set_size_hint

```c
void iterator_set_size_hint(iterator *iter, size_t size_hint);
```

- Parameters: `iter`, `size_hint`
- Returns: none.
- Notes: sets a conservative remaining-item count used by collect helpers to
  reserve output storage.

### iterator_size_hint

```c
size_t iterator_size_hint(const iterator *iter);
```

- Parameters: `iter`
- Returns: remaining-item hint, or zero when unknown.

### iterator_collect

```c
status iterator_collect(iterator *source,
                              size_t elem_size,
                              allocator *allocator,
                              vector **out);
```

- Parameters: `source`, `elem_size`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: vector containing copies of the remaining source items to `*out`.
- Notes: consumes `source`. The returned vector owns its storage and can outlive
  the original data structure.
- Example:

```c
vector *values = NULL;
if (vector_create(sizeof(int), NULL, &values) != STATUS_OK) {
    /* handle allocation failure */
}
int one = 1;
int two = 2;
if (vector_push(values, &one) != STATUS_OK) {
    /* handle allocation failure */
}
if (vector_push(values, &two) != STATUS_OK) {
    /* handle allocation failure */
}

iterator iter = vector_get_iterator(values);
vector *copy = NULL;
if (iterator_collect(&iter, sizeof(int), NULL, &copy) != STATUS_OK) {
    /* handle allocation failure */
}

vector_destroy(values);
/* copy still owns the collected ints. */
vector_destroy(copy);
```

### iterator_collect_map

```c
status iterator_collect_map(iterator *source,
                                  size_t dst_elem_size,
                                  iterator_map_into_fn map,
                                  void *context,
                                  allocator *allocator,
                                  vector **out);
```

- Parameters: `source`, `dst_elem_size`, `map`, `context`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: vector containing mapped copies of the remaining source items to `*out`.
- Notes: consumes `source`. Use this when the output element type or size is
  different from the source element type.
- Example:

```c
static void int_to_double(void *context, const void *src, void *dst) {
    (void)context;
    *(double *)dst = (double)*(const int *)src;
}

vector *ints = NULL;
if (vector_create(sizeof(int), NULL, &ints) != STATUS_OK) {
    /* handle allocation failure */
}
int value = 42;
if (vector_push(ints, &value) != STATUS_OK) {
    /* handle allocation failure */
}

iterator iter = vector_get_iterator(ints);
vector *doubles = NULL;
if (iterator_collect_map(&iter, sizeof(double), int_to_double, NULL, NULL, &doubles) != STATUS_OK) {
    /* handle allocation failure */
}

const double *mapped = (const double *)vector_get_const(doubles, 0);
/* *mapped == 42.0 */

vector_destroy(ints);
vector_destroy(doubles);
```
