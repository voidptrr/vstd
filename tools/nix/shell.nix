{pkgs}:
with pkgs;
  mkShell {
    packages = [
      gcc
      cmake
      ninja
      clang-tools
      alejandra
      (python3.withPackages (ps: [
        ps.mkdocs
        ps.mkdocs-material
      ]))
    ];
  }
