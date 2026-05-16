---
name: benchmark-doc-sync
description: Synchronize data-structure benchmark tables in docs with current benchmark execution output.
version: 1.0.0
allowed-tools:
  - Read
  - Grep
  - Bash
---

## Overview

This skill keeps benchmark documentation accurate by extracting current benchmark results and updating docs tables.

## When to Use

- Benchmark implementations change.
- Benchmark cases are added/renamed/removed.
- Docs benchmark values appear stale.

## Process

1. Run `nix run .#bench-all` and capture stdout values.
2. Extract median `ns/op` and `ops/sec` for each documented case.
3. Update each data-structure doc table with exact case names and current values.
4. Remove obsolete cases and add newly introduced cases.
5. Rebuild docs with `nix run .#docs-build`.

## Rationalizations

| Rationalization | Why it is wrong | Correct action |
|---|---|---|
| "Approximate numbers are fine." | Drift erodes trust in documentation quality. | Copy exact current run values.
| "Only one table changed; skip global check." | Case sets can diverge silently across pages. | Verify all DS pages touched by benchmarks.

## Red Flags

- Docs contain benchmark cases absent from runtime output.
- Runtime output includes cases missing in docs tables.
- Docs build fails after table edits.

## Verification

- Run `nix run .#bench-all`.
- Run `nix run .#docs-build`.
- Final report includes updated files and the benchmark cases changed.
