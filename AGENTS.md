---
schema_version: 1
project: cstd
defaults:
  validation_commands:
    - nix develop -c format-all
    - nix develop -c test-all
    - nix develop -c docs-build
  constraints:
    - Use canonical includes under cstd/ (for example cstd/datastruct/queue.h and cstd/status.h).
    - Do not introduce benchmark log artifact files.
    - Keep API behavior consistent across containers (null handling and empty handling).
    - Follow commit subject style used in this repo: prefix-first lowercase, such as ds(queue): ..., bench: ..., docs: ..., chore: ....
  output_style:
    tone: concise
    include:
      - changed_files
      - validation_summary
skill_pack:
  format: skill-md
  entrypoint: .agents/skills/using-agent-skills/SKILL.md
  required_sections:
    - Overview
    - When to Use
    - Process
    - Rationalizations
    - Red Flags
    - Verification
agents:
  - id: code-reviewer
    role: API consistency reviewer
    skill: .agents/skills/api-consistency/SKILL.md
  - id: docs-benchmark-sync
    role: Benchmark table synchronizer
    skill: .agents/skills/benchmark-doc-sync/SKILL.md
---

Machine-consumable agent policy for this repository.
Skills live under `.agents/skills/*/SKILL.md`.
