# cstd

Personal C standard-library style workspace for reusable data structures, algorithms, and utilities.

## Goals

- Build a small, explicit, test-driven "personal std" for C.
- Keep APIs stable and easy to reason about.
- Favor clear behavior and strong invariants over clever abstractions.

## Module Status

| Module | Implemented |
| --- | --- |
| `cstd_status` | Yes |
| `cstd_vector` | Yes |
| `cstd_queue` | Yes |
| `cstd_stack` | No |
| `cstd_deque` | No |
| `cstd_map` | No |
| `cstd_set` | No |
| `cstd_string` | No |
| `cstd_allocator` | No |

## Development

- Format: `nix develop -c format-check`
- Hardened compile + tests: `nix develop -c compile-hard`
- Full tests: `nix develop -c test-all`
