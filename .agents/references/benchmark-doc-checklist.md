# Benchmark Doc Checklist

- Run `nix develop -c bench-all` and use current stdout values.
- Match benchmark case names exactly.
- Update median `ns/op` and `ops/sec` for each DS page.
- Run `nix develop -c docs-build` after edits.
