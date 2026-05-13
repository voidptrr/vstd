{pkgs}:
pkgs.writeShellApplication {
  name = "format-check";
  runtimeInputs = [
    pkgs.alejandra
    pkgs.clang-tools
    pkgs.findutils
    pkgs.coreutils
  ];
  text = ''
    set -eu

    alejandra --check .

    files="$(find src tests include -type f \( -name '*.c' -o -name '*.h' \) | sort)"
    if [ -z "$files" ]; then
      exit 0
    fi

    while IFS= read -r file; do
      [ -z "$file" ] && continue
      clang-format --dry-run --Werror "$file"
    done <<EOF
    $files
    EOF
  '';
}
