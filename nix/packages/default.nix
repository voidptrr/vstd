{pkgs}: {
  code-check = import ./compile-hard.nix {inherit pkgs;};
  format-all = import ./format-all.nix {inherit pkgs;};
  format-check = import ./format-check.nix {inherit pkgs;};
  test-all = import ./test-all.nix {inherit pkgs;};
}
