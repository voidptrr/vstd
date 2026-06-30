# ds.k4c_linked_list

## DESCRIPTION

The k4c_linked_list module provides an intrusive singly linked list. User objects embed
`k4c_linked_list_node`, and the list links those embedded nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `K4C_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### k4c_linked_list_node

```c
typedef struct k4c_linked_list_node {
    struct k4c_linked_list_node *next;
} k4c_linked_list_node;
```

Embed this node in the object you want to link.

### k4c_linked_list_iterator

```c
typedef struct k4c_linked_list_iterator k4c_linked_list_iterator;
```

Typed cursor for walking linked-list nodes.

### K4C_CONTAINER_OF

```c
#define K4C_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.
- Example:

```c
job *item = K4C_CONTAINER_OF(node, job, node);
```

## FUNCTIONS

### k4c_linked_list_create

```c
k4c_status k4c_linked_list_create(k4c_allocator *k4c_allocator, k4c_linked_list **out);
```

- Parameters: `k4c_allocator`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: opaque linked-list handle to `*out` on success.
- Notes: the list copies `k4c_allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `k4c_allocator` is `NULL`, the handle uses the C
  library k4c_heap through `k4c_alloc`.
- Example:

```c
k4c_linked_list *list = NULL;
if (k4c_linked_list_create(NULL, &list) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_linked_list_push

```c
void k4c_linked_list_push(k4c_linked_list *list, k4c_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Example:

```c
job item = {.id = 1};
k4c_linked_list_push(list, &item.node);
```

### k4c_linked_list_pushfront

```c
void k4c_linked_list_pushfront(k4c_linked_list *list, k4c_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Example:

```c
job item = {.id = 1};
k4c_linked_list_pushfront(list, &item.node);
```

### k4c_linked_list_popleft

```c
k4c_linked_list_node *k4c_linked_list_popleft(k4c_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.
- Example:

```c
k4c_linked_list_node *node = k4c_linked_list_popleft(list);
if (node != NULL) {
    job *item = K4C_CONTAINER_OF(node, job, node);
}
```

### k4c_linked_list_remove_after

```c
k4c_linked_list_node *k4c_linked_list_remove_after(k4c_linked_list *list,
                                                     k4c_linked_list_node *prev);
```

- Parameters: `list`, `prev`
- Returns: removed node after `prev`, or removed head node when `prev` is `NULL`.
- Notes: returns `NULL` when there is no node to remove.
- Example:

```c
k4c_linked_list_node *node = k4c_linked_list_remove_after(list, prev);
```

### k4c_linked_list_size

```c
size_t k4c_linked_list_size(const k4c_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Example:

```c
size_t count = k4c_linked_list_size(list);
```

### k4c_linked_list_head

```c
k4c_linked_list_node *k4c_linked_list_head(const k4c_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.
- Example:

```c
k4c_linked_list_node *head = k4c_linked_list_head(list);
```

### k4c_linked_list_get_iterator

```c
k4c_iterator k4c_linked_list_get_iterator(const k4c_linked_list *list);
```

- Parameters: `list`
- Returns: k4c_iterator over list nodes from head to tail.
- Notes: yielded pointers are `const k4c_linked_list_node *`. Use
  `K4C_CONTAINER_OF` to recover the owning object.
- Example:

```c
k4c_iterator iter = k4c_linked_list_get_iterator(list);

const k4c_linked_list_node *node;
while ((node = (const k4c_linked_list_node *)k4c_iterator_next(&iter)) != NULL) {
    const job *item = K4C_CONTAINER_OF(node, job, node);
}
```

### k4c_linked_list_destroy

```c
void k4c_linked_list_destroy(k4c_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.
- Example:

```c
k4c_linked_list_destroy(list);
```
