{pkgs}:
pkgs.writeShellApplication {
  name = "docs-build";
  runtimeInputs = [
    (pkgs.python3.withPackages (ps: [
      ps.mkdocs
      ps.mkdocs-material
    ]))
  ];
  text = ''
    set -eu

    cd docs
    mkdocs build --strict
  '';
}
