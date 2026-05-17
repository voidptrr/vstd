{pkgs}: {
  bench-all = import ./bench-all.nix {inherit pkgs;};
  code-check = import ./code-check.nix {inherit pkgs;};
  docs-build = import ./docs-build.nix {inherit pkgs;};
  docs-dev = import ./docs-dev.nix {inherit pkgs;};
  format-all = import ./format-all.nix {inherit pkgs;};
  format-check = import ./format-check.nix {inherit pkgs;};
  test-all = import ./test-all.nix {inherit pkgs;};
}
