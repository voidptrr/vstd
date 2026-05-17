{pkgs}:
pkgs.writeShellApplication {
  name = "docs-dev";
  runtimeInputs = [
    (pkgs.python3.withPackages (ps: [
      ps.mkdocs
      ps.mkdocs-material
    ]))
  ];
  text = ''
    set -eu

    cd docs
    mkdocs serve --dev-addr 0.0.0.0:3000
  '';
}
