# Benchmark Doc Checklist

- Run `nix run .#bench-all` and use current stdout values.
- Match benchmark case names exactly.
- Update median `ns/op` and `ops/sec` for each DS page.
- Run `nix run .#docs-build` after edits.
