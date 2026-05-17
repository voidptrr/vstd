{pkgs}:
pkgs.writeShellApplication {
  name = "code-check";
  runtimeInputs = [
    pkgs.cmake
    pkgs.gcc
    pkgs.gnumake
  ];
  text = ''
    set -eu

    hardening_flags="-O1 -g3 -fno-omit-frame-pointer -fstack-protector-strong -D_FORTIFY_SOURCE=3 -fPIE -fsanitize=address,undefined"
    hardening_linker_flags="-Wl,-z,relro,-z,now -pie -fsanitize=address,undefined"

    cmake \
      -S . \
      -B build/hardened \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCSTD_BUILD_BENCHMARKS=ON \
      -DCMAKE_C_FLAGS="$hardening_flags" \
      -DCMAKE_EXE_LINKER_FLAGS="$hardening_linker_flags"

    cmake --build build/hardened
    ctest --test-dir build/hardened --output-on-failure
  '';
}
