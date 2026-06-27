# ds.linked_list

## DESCRIPTION

The linked_list module provides an intrusive singly linked list. User objects embed
`linked_list_node`, and the list links those embedded nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### linked_list_node

```c
typedef struct linked_list_node {
    struct linked_list_node *next;
} linked_list_node;
```

Embed this node in the object you want to link.

### linked_list_iterator

```c
typedef struct linked_list_iterator linked_list_iterator;
```

Typed cursor for walking linked-list nodes.

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

### linked_list_create

```c
status linked_list_create(allocator *allocator, linked_list **out);
```

- Parameters: `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: opaque linked-list handle to `*out` on success.
- Notes: the list stores `allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `allocator` is `NULL`, the handle uses the C
  library heap through `alloc`.
- Example:

```c
linked_list *list = NULL;
if (linked_list_create(NULL, &list) != STATUS_OK) {
    /* handle allocation failure */
}
```

### linked_list_push

```c
void linked_list_push(linked_list *list, linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Example:

```c
job item = {.id = 1};
linked_list_push(list, &item.node);
```

### linked_list_pushfront

```c
void linked_list_pushfront(linked_list *list, linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Example:

```c
job item = {.id = 1};
linked_list_pushfront(list, &item.node);
```

### linked_list_popleft

```c
linked_list_node *linked_list_popleft(linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.
- Example:

```c
linked_list_node *node = linked_list_popleft(list);
if (node != NULL) {
    job *item = CONTAINER_OF(node, job, node);
}
```

### linked_list_remove_after

```c
linked_list_node *linked_list_remove_after(linked_list *list,
                                                     linked_list_node *prev);
```

- Parameters: `list`, `prev`
- Returns: removed node after `prev`, or removed head node when `prev` is `NULL`.
- Notes: returns `NULL` when there is no node to remove.
- Example:

```c
linked_list_node *node = linked_list_remove_after(list, prev);
```

### linked_list_size

```c
size_t linked_list_size(const linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Example:

```c
size_t count = linked_list_size(list);
```

### linked_list_head

```c
linked_list_node *linked_list_head(const linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.
- Example:

```c
linked_list_node *head = linked_list_head(list);
```

### linked_list_get_iterator

```c
iterator linked_list_get_iterator(const linked_list *list);
```

- Parameters: `list`
- Returns: iterator over list nodes from head to tail.
- Notes: yielded pointers are `const linked_list_node *`. Use
  `CONTAINER_OF` to recover the owning object.
- Example:

```c
iterator iter = linked_list_get_iterator(list);

const linked_list_node *node;
while ((node = (const linked_list_node *)iterator_next(&iter)) != NULL) {
    const job *item = CONTAINER_OF(node, job, node);
}
```

### linked_list_destroy

```c
void linked_list_destroy(linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.
- Example:

```c
linked_list_destroy(list);
```
