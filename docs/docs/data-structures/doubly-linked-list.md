# ds.k4c_doubly_linked_list

## DESCRIPTION

The k4c_doubly_linked_list module provides an intrusive doubly linked list. User
objects embed `k4c_doubly_linked_list_node`, and the list links those embedded
nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `K4C_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### k4c_doubly_linked_list_node

```c
typedef struct k4c_doubly_linked_list_node {
    struct k4c_doubly_linked_list_node *prev;
    struct k4c_doubly_linked_list_node *next;
} k4c_doubly_linked_list_node;
```

Embed this node in the object you want to link.

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

### k4c_doubly_linked_list_create

```c
k4c_status k4c_doubly_linked_list_create(k4c_allocator *k4c_allocator, k4c_doubly_linked_list **out);
```

- Parameters: `k4c_allocator`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: opaque doubly linked-list handle to `*out` on success.
- Notes: the list copies `k4c_allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `k4c_allocator` is `NULL`, the handle uses the C
  library k4c_heap through `k4c_alloc`.
- Example:

```c
k4c_doubly_linked_list *list = NULL;
if (k4c_doubly_linked_list_create(NULL, &list) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_doubly_linked_list_push

```c
void k4c_doubly_linked_list_push(k4c_doubly_linked_list *list,
                                  k4c_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: appends `node` at the tail.
- Example:

```c
job item = {.id = 1};
k4c_doubly_linked_list_push(list, &item.node);
```

### k4c_doubly_linked_list_pushfront

```c
void k4c_doubly_linked_list_pushfront(k4c_doubly_linked_list *list,
                                       k4c_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: prepends `node` at the head.
- Example:

```c
job item = {.id = 1};
k4c_doubly_linked_list_pushfront(list, &item.node);
```

### k4c_doubly_linked_list_insert_after

```c
void k4c_doubly_linked_list_insert_after(k4c_doubly_linked_list *list,
                                          k4c_doubly_linked_list_node *after,
                                          k4c_doubly_linked_list_node *node);
```

- Parameters: `list`, `after`, `node`
- Returns: none.
- Notes: inserts `node` after `after`, or at the front when `after` is `NULL`.
- Example:

```c
k4c_doubly_linked_list_insert_after(list, &first.node, &second.node);
```

### k4c_doubly_linked_list_popleft

```c
k4c_doubly_linked_list_node *k4c_doubly_linked_list_popleft(k4c_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.
- Example:

```c
k4c_doubly_linked_list_node *node = k4c_doubly_linked_list_popleft(list);
```

### k4c_doubly_linked_list_popback

```c
k4c_doubly_linked_list_node *k4c_doubly_linked_list_popback(k4c_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed tail node, or `NULL` when list is empty.
- Example:

```c
k4c_doubly_linked_list_node *node = k4c_doubly_linked_list_popback(list);
```

### k4c_doubly_linked_list_remove

```c
void k4c_doubly_linked_list_remove(k4c_doubly_linked_list *list,
                                    k4c_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: unlinks a known node in `O(1)`.
- Example:

```c
k4c_doubly_linked_list_remove(list, &item.node);
```

### k4c_doubly_linked_list_size

```c
size_t k4c_doubly_linked_list_size(const k4c_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Example:

```c
size_t count = k4c_doubly_linked_list_size(list);
```

### k4c_doubly_linked_list_head

```c
k4c_doubly_linked_list_node *k4c_doubly_linked_list_head(const k4c_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.
- Example:

```c
k4c_doubly_linked_list_node *head = k4c_doubly_linked_list_head(list);
```

### k4c_doubly_linked_list_tail

```c
k4c_doubly_linked_list_node *k4c_doubly_linked_list_tail(const k4c_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current tail node, or `NULL` when list is empty.
- Example:

```c
k4c_doubly_linked_list_node *tail = k4c_doubly_linked_list_tail(list);
```

### k4c_doubly_linked_list_get_iterator

```c
k4c_iterator k4c_doubly_linked_list_get_iterator(const k4c_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: k4c_iterator over list nodes from head to tail.
- Notes: yielded pointers are `const k4c_doubly_linked_list_node *`. Use
  `K4C_CONTAINER_OF` to recover the owning object.
- Example:

```c
k4c_iterator iter = k4c_doubly_linked_list_get_iterator(list);

const k4c_doubly_linked_list_node *node;
while ((node = (const k4c_doubly_linked_list_node *)k4c_iterator_next(&iter)) != NULL) {
    const job *item = K4C_CONTAINER_OF(node, job, node);
}
```

### k4c_doubly_linked_list_destroy

```c
void k4c_doubly_linked_list_destroy(k4c_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.
- Example:

```c
k4c_doubly_linked_list_destroy(list);
```
