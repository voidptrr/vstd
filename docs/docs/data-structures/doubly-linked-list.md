# datastruct.doubly_linked_list

## DESCRIPTION

The doubly_linked_list module provides an intrusive doubly linked list. User
objects embed `vs_doubly_linked_list_node`, and the list links those embedded
nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `VS_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_doubly_linked_list_node

```c
typedef struct vs_doubly_linked_list_node {
    struct vs_doubly_linked_list_node *prev;
    struct vs_doubly_linked_list_node *next;
} vs_doubly_linked_list_node;
```

Embed this node in the object you want to link.

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

### vs_doubly_linked_list_create

```c
vs_status vs_doubly_linked_list_create(vs_allocator *allocator, vs_doubly_linked_list **out);
```

- Parameters: `allocator`, `out`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Writes: opaque doubly linked-list handle to `*out` on success.
- Notes: the list stores `allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `allocator` is `NULL`, the handle uses the C
  library heap through `vs_alloc`.
- Example:

```c
vs_doubly_linked_list *list = NULL;
if (vs_doubly_linked_list_create(NULL, &list) != VS_STATUS_OK) {
    /* handle allocation failure */
}
```

### vs_doubly_linked_list_push

```c
void vs_doubly_linked_list_push(vs_doubly_linked_list *list,
                                  vs_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: appends `node` at the tail.
- Example:

```c
job item = {.id = 1};
vs_doubly_linked_list_push(list, &item.node);
```

### vs_doubly_linked_list_pushfront

```c
void vs_doubly_linked_list_pushfront(vs_doubly_linked_list *list,
                                       vs_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: prepends `node` at the head.
- Example:

```c
job item = {.id = 1};
vs_doubly_linked_list_pushfront(list, &item.node);
```

### vs_doubly_linked_list_insert_after

```c
void vs_doubly_linked_list_insert_after(vs_doubly_linked_list *list,
                                          vs_doubly_linked_list_node *after,
                                          vs_doubly_linked_list_node *node);
```

- Parameters: `list`, `after`, `node`
- Returns: none.
- Notes: inserts `node` after `after`, or at the front when `after` is `NULL`.
- Example:

```c
vs_doubly_linked_list_insert_after(list, &first.node, &second.node);
```

### vs_doubly_linked_list_popleft

```c
vs_doubly_linked_list_node *vs_doubly_linked_list_popleft(vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.
- Example:

```c
vs_doubly_linked_list_node *node = vs_doubly_linked_list_popleft(list);
```

### vs_doubly_linked_list_popback

```c
vs_doubly_linked_list_node *vs_doubly_linked_list_popback(vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed tail node, or `NULL` when list is empty.
- Example:

```c
vs_doubly_linked_list_node *node = vs_doubly_linked_list_popback(list);
```

### vs_doubly_linked_list_remove

```c
void vs_doubly_linked_list_remove(vs_doubly_linked_list *list,
                                    vs_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: unlinks a known node in `O(1)`.
- Example:

```c
vs_doubly_linked_list_remove(list, &item.node);
```

### vs_doubly_linked_list_size

```c
size_t vs_doubly_linked_list_size(const vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Example:

```c
size_t count = vs_doubly_linked_list_size(list);
```

### vs_doubly_linked_list_head

```c
vs_doubly_linked_list_node *vs_doubly_linked_list_head(const vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.
- Example:

```c
vs_doubly_linked_list_node *head = vs_doubly_linked_list_head(list);
```

### vs_doubly_linked_list_tail

```c
vs_doubly_linked_list_node *vs_doubly_linked_list_tail(const vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current tail node, or `NULL` when list is empty.
- Example:

```c
vs_doubly_linked_list_node *tail = vs_doubly_linked_list_tail(list);
```

### vs_doubly_linked_list_get_iterator

```c
vs_iterator vs_doubly_linked_list_get_iterator(const vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: iterator over list nodes from head to tail.
- Notes: yielded pointers are `const vs_doubly_linked_list_node *`. Use
  `VS_CONTAINER_OF` to recover the owning object.
- Example:

```c
vs_iterator iter = vs_doubly_linked_list_get_iterator(list);

const vs_doubly_linked_list_node *node;
while ((node = (const vs_doubly_linked_list_node *)vs_iterator_next(&iter)) != NULL) {
    const job *item = VS_CONTAINER_OF(node, job, node);
}
```

### vs_doubly_linked_list_destroy

```c
void vs_doubly_linked_list_destroy(vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.
- Example:

```c
vs_doubly_linked_list_destroy(list);
```
