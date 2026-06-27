# datastruct.linked_list

## DESCRIPTION

The linked_list module provides an intrusive singly linked list. User objects embed
`vs_linked_list_node`, and the list links those embedded nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `VS_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_linked_list_node

```c
typedef struct vs_linked_list_node {
    struct vs_linked_list_node *next;
} vs_linked_list_node;
```

Embed this node in the object you want to link.

### vs_linked_list_iterator

```c
typedef struct vs_linked_list_iterator vs_linked_list_iterator;
```

Typed cursor for walking linked-list nodes.

### VS_CONTAINER_OF

```c
#define VS_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.
- Example:

```c
job *item = VS_CONTAINER_OF(node, job, node);
```

## FUNCTIONS

### vs_linked_list_create

```c
vs_status vs_linked_list_create(vs_allocator *allocator, vs_linked_list **out);
```

- Parameters: `allocator`, `out`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Writes: opaque linked-list handle to `*out` on success.
- Notes: the list stores `allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `allocator` is `NULL`, the handle uses the C
  library heap through `vs_alloc`.
- Example:

```c
vs_linked_list *list = NULL;
if (vs_linked_list_create(NULL, &list) != VS_STATUS_OK) {
    /* handle allocation failure */
}
```

### vs_linked_list_push

```c
void vs_linked_list_push(vs_linked_list *list, vs_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Example:

```c
job item = {.id = 1};
vs_linked_list_push(list, &item.node);
```

### vs_linked_list_pushfront

```c
void vs_linked_list_pushfront(vs_linked_list *list, vs_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Example:

```c
job item = {.id = 1};
vs_linked_list_pushfront(list, &item.node);
```

### vs_linked_list_popleft

```c
vs_linked_list_node *vs_linked_list_popleft(vs_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.
- Example:

```c
vs_linked_list_node *node = vs_linked_list_popleft(list);
if (node != NULL) {
    job *item = VS_CONTAINER_OF(node, job, node);
}
```

### vs_linked_list_remove_after

```c
vs_linked_list_node *vs_linked_list_remove_after(vs_linked_list *list,
                                                     vs_linked_list_node *prev);
```

- Parameters: `list`, `prev`
- Returns: removed node after `prev`, or removed head node when `prev` is `NULL`.
- Notes: returns `NULL` when there is no node to remove.
- Example:

```c
vs_linked_list_node *node = vs_linked_list_remove_after(list, prev);
```

### vs_linked_list_size

```c
size_t vs_linked_list_size(const vs_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Example:

```c
size_t count = vs_linked_list_size(list);
```

### vs_linked_list_head

```c
vs_linked_list_node *vs_linked_list_head(const vs_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.
- Example:

```c
vs_linked_list_node *head = vs_linked_list_head(list);
```

### vs_linked_list_get_iterator

```c
vs_iterator vs_linked_list_get_iterator(const vs_linked_list *list);
```

- Parameters: `list`
- Returns: iterator over list nodes from head to tail.
- Notes: yielded pointers are `const vs_linked_list_node *`. Use
  `VS_CONTAINER_OF` to recover the owning object.
- Example:

```c
vs_iterator iter = vs_linked_list_get_iterator(list);

const vs_linked_list_node *node;
while ((node = (const vs_linked_list_node *)vs_iterator_next(&iter)) != NULL) {
    const job *item = VS_CONTAINER_OF(node, job, node);
}
```

### vs_linked_list_destroy

```c
void vs_linked_list_destroy(vs_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.
- Example:

```c
vs_linked_list_destroy(list);
```
