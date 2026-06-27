# ds.doubly_linked_list

## DESCRIPTION

The doubly_linked_list module provides an intrusive doubly linked list. User
objects embed `doubly_linked_list_node`, and the list links those embedded
nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### doubly_linked_list_node

```c
typedef struct doubly_linked_list_node {
    struct doubly_linked_list_node *prev;
    struct doubly_linked_list_node *next;
} doubly_linked_list_node;
```

Embed this node in the object you want to link.

### CONTAINER_OF

```c
#define CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.
- Example:

```c
job *item = CONTAINER_OF(node, job, node);
```

## FUNCTIONS

### doubly_linked_list_create

```c
status doubly_linked_list_create(allocator *allocator, doubly_linked_list **out);
```

- Parameters: `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: opaque doubly linked-list handle to `*out` on success.
- Notes: the list stores `allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `allocator` is `NULL`, the handle uses the C
  library heap through `alloc`.
- Example:

```c
doubly_linked_list *list = NULL;
if (doubly_linked_list_create(NULL, &list) != STATUS_OK) {
    /* handle allocation failure */
}
```

### doubly_linked_list_push

```c
void doubly_linked_list_push(doubly_linked_list *list,
                                  doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: appends `node` at the tail.
- Example:

```c
job item = {.id = 1};
doubly_linked_list_push(list, &item.node);
```

### doubly_linked_list_pushfront

```c
void doubly_linked_list_pushfront(doubly_linked_list *list,
                                       doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: prepends `node` at the head.
- Example:

```c
job item = {.id = 1};
doubly_linked_list_pushfront(list, &item.node);
```

### doubly_linked_list_insert_after

```c
void doubly_linked_list_insert_after(doubly_linked_list *list,
                                          doubly_linked_list_node *after,
                                          doubly_linked_list_node *node);
```

- Parameters: `list`, `after`, `node`
- Returns: none.
- Notes: inserts `node` after `after`, or at the front when `after` is `NULL`.
- Example:

```c
doubly_linked_list_insert_after(list, &first.node, &second.node);
```

### doubly_linked_list_popleft

```c
doubly_linked_list_node *doubly_linked_list_popleft(doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.
- Example:

```c
doubly_linked_list_node *node = doubly_linked_list_popleft(list);
```

### doubly_linked_list_popback

```c
doubly_linked_list_node *doubly_linked_list_popback(doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed tail node, or `NULL` when list is empty.
- Example:

```c
doubly_linked_list_node *node = doubly_linked_list_popback(list);
```

### doubly_linked_list_remove

```c
void doubly_linked_list_remove(doubly_linked_list *list,
                                    doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: unlinks a known node in `O(1)`.
- Example:

```c
doubly_linked_list_remove(list, &item.node);
```

### doubly_linked_list_size

```c
size_t doubly_linked_list_size(const doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Example:

```c
size_t count = doubly_linked_list_size(list);
```

### doubly_linked_list_head

```c
doubly_linked_list_node *doubly_linked_list_head(const doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.
- Example:

```c
doubly_linked_list_node *head = doubly_linked_list_head(list);
```

### doubly_linked_list_tail

```c
doubly_linked_list_node *doubly_linked_list_tail(const doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current tail node, or `NULL` when list is empty.
- Example:

```c
doubly_linked_list_node *tail = doubly_linked_list_tail(list);
```

### doubly_linked_list_get_iterator

```c
iterator doubly_linked_list_get_iterator(const doubly_linked_list *list);
```

- Parameters: `list`
- Returns: iterator over list nodes from head to tail.
- Notes: yielded pointers are `const doubly_linked_list_node *`. Use
  `CONTAINER_OF` to recover the owning object.
- Example:

```c
iterator iter = doubly_linked_list_get_iterator(list);

const doubly_linked_list_node *node;
while ((node = (const doubly_linked_list_node *)iterator_next(&iter)) != NULL) {
    const job *item = CONTAINER_OF(node, job, node);
}
```

### doubly_linked_list_destroy

```c
void doubly_linked_list_destroy(doubly_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.
- Example:

```c
doubly_linked_list_destroy(list);
```
