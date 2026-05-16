---
name: using-agent-skills
description: Route work to the correct cstd skill and enforce required verification and output conventions.
version: 1.0.0
allowed-tools:
  - Read
  - Grep
  - Glob
---

## Overview

This meta-skill maps incoming requests to the correct repository skill and ensures consistent execution and reporting.

## When to Use

- At the start of a task where the best specialized skill is unclear.
- When work spans API behavior and docs/benchmark synchronization.

## Process

1. Classify the request as API-focused, benchmark-doc-focused, or mixed.
2. Load `.agents/skills/api-consistency/SKILL.md` for API behavior changes.
3. Load `.agents/skills/benchmark-doc-sync/SKILL.md` for benchmark table updates.
4. If mixed, run API consistency first, then benchmark-doc sync.
5. Enforce repository validation commands before final output when code or docs changed.
6. When creating commits, enforce repository commit subject style: lowercase prefix-first format such as `ds(queue): ...`, `bench: ...`, `docs: ...`, `chore: ...`.

## Rationalizations

| Rationalization | Why it is wrong | Correct action |
|---|---|---|
| "I can skip skill routing for small edits." | Small edits can still break consistency contracts. | Route to a skill first, then execute.
| "Bench values probably did not change." | Benchmark variance and code changes can invalidate docs quickly. | Re-run benchmarks before docs sync.

## Red Flags

- API status codes differ for equivalent empty/null behavior.
- Docs benchmark tables do not match latest `bench-all` output.
- Final report omits changed files or validation summary.
- Commit subjects do not follow repo prefix-first style.

## Verification

- Confirm selected skill file path is referenced in the final work plan.
- Run required validations from repository defaults when changes were made.
- Verify each new commit subject matches the repository prefix-first naming convention.
