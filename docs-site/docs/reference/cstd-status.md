# cstd.status

`cstd.status` is the canonical status code set for `cstd` APIs.

## Contract

- Stateful operations return `cstd_status`.
- `CSTD_OK` indicates success.
- Non-`CSTD_OK` values indicate a specific failure class.

| Code | Meaning | Typical trigger |
|---|---|---|
| `CSTD_OK` | Success | Operation completed |
| `CSTD_ERR_NULL` | Null pointer input | Required input pointer is `NULL` |
| `CSTD_ERR_EMPTY` | Empty container state | Pop or peek on an empty container |
| `CSTD_ERR_RANGE` | Invalid value range | `elem_size == 0` in init |
| `CSTD_ERR_OOM` | Allocation failure | Memory allocation/growth failure |
| `CSTD_ERR_STATE` | Invalid runtime state | Internal state does not permit operation |
