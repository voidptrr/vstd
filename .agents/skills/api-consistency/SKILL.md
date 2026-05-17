---
name: api-consistency
description: Validate and improve consistency, return semantics, and usability across cstd container APIs.
version: 1.0.0
allowed-tools:
  - Read
  - Grep
  - Glob
  - Bash
---

## Overview

This skill reviews public API contracts for vector, queue, and linked_list to keep behavior uniform and easy to consume.

## When to Use

- Public headers in `include/cstd/datastruct` change.
- New container operations are added.
- Return/status behavior is modified.

## Process

1. Enumerate changed header/source pairs for all affected containers.
2. Compare operation families across containers (`push`, `pop`, `peek`, `is_empty`, `size`, `free`, `init`).
3. Verify equivalent conditions return equivalent status codes.
4. Ensure null-handling rules are consistent for pointer inputs and outputs.
5. Add or update tests for each changed behavioral contract.
6. Update docs API tables/examples when public behavior changes.

## Rationalizations

| Rationalization | Why it is wrong | Correct action |
|---|---|---|
| "This container is special; inconsistency is fine." | Inconsistent APIs increase caller complexity and bugs. | Keep parity unless explicitly documented and justified.
| "Tests already pass so API is good." | Passing tests can still miss parity and usability regressions. | Add parity-focused tests and review all siblings.

## Red Flags

- Empty-condition returns differ between equivalent pop/peek operations.
- One container has a new operation without sibling parity rationale.
- Docs API tables do not reflect the actual header contract.

## Verification

- Run `nix develop -c test-all`.
- Confirm docs build if docs changed: `nix develop -c docs-build`.
- Final report includes issues found, fixes applied, and files touched.
