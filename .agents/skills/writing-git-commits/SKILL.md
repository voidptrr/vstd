---
name: writing-git-commits
description: Write git commit messages. Activate when user ask to write commit, draft commit or similar.
---

## Format

```text
<subsystem>: <summary>
<long form description>
```

## Rules

### Subject line

- Use a subsystem prefix. The subsystem must be a lowercase identifier for the area of code changes (for example: `datastruct`, `mem`, `docs`, `tools`). Determine it from file paths in the diff.
- Start the summary in lowercase, use imperative mood, and do not end with a period.
- Keep the full subject line under 60 characters.

### Long form description

- Explain what changed, what the previous behavior was, and how the new behavior works.
- Write in plain prose, not bullet points.
- Focus on why and how.
- Use a direct, technical tone with no filler phrases.
- Keep it to no more than a handful of paragraphs.

### Workflow

- Run `fid` to inspect changes since the last commit.
- Identify the subsystem from changed file paths.
- Draft the commit message using the format above.
- Apply the commit.
- Do not push the commit; leave pushing to the user.
