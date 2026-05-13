{pkgs}:
pkgs.writeShellApplication {
  name = "format-all";
  runtimeInputs = [
    pkgs.alejandra
    pkgs.clang-tools
    pkgs.findutils
  ];
  text = ''
    set -eu

    alejandra .

    find src tests include -type f \( -name '*.c' -o -name '*.h' \) -print0 | xargs -0 --no-run-if-empty clang-format -i
  '';
}
