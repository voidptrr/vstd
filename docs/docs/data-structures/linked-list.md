# datastruct.linked_list

## DESCRIPTION

The linked_list module provides a generic singly linked list for fixed-size elements.
It supports insertion at head and tail and removal from head.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ckit_linked_list_init

```c
void ckit_linked_list_init(ckit_linked_list *list, size_t elem_size,
                           ckit_allocator *allocator);
```

- Parameters: `list`, `elem_size`, `allocator`
- Returns: none.
- Notes: when `allocator` is `NULL`, linked list uses default `ckit_malloc` backing.

### ckit_linked_list_push

```c
void ckit_linked_list_push(ckit_linked_list *list, const void *element);
```

- Parameters: `list`, `element`
- Returns: none.

### ckit_linked_list_pushfront

```c
void ckit_linked_list_pushfront(ckit_linked_list *list, const void *element);
```

- Parameters: `list`, `element`
- Returns: none.

### ckit_linked_list_popleft

```c
void *ckit_linked_list_popleft(ckit_linked_list *list);
```

- Parameters: `list`
- Returns: pointer to removed front element data, or `NULL` when list is empty.

### ckit_linked_list_free

```c
void ckit_linked_list_free(ckit_linked_list *list);
```

- Parameters: `list`
- Returns: none.

### ckit_linked_list_size

```c
size_t ckit_linked_list_size(const ckit_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Notes: returns `0` when `list` is `NULL`.

### ckit_linked_list_is_empty

```c
bool ckit_linked_list_is_empty(const ckit_linked_list *list);
```

- Parameters: `list`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `list` is `NULL`.
