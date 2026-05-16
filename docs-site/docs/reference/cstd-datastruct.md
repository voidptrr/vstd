# cstd.datastruct

`cstd.datastruct` is the container module namespace for `cstd`.

## Behavior contract

- Stateful APIs return `cstd_status`.
- Container lifecycle is explicit: `init -> use -> free`.
- Null and empty semantics are aligned across modules.
- Storage is generic and copy-based via `elem_size`.

## Available modules

- `cstd.datastruct.vector`
- `cstd.datastruct.queue`
- `cstd.datastruct.linked_list`
