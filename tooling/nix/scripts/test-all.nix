{pkgs}:
pkgs.writeShellApplication {
  name = "test-all";
  runtimeInputs = [
    pkgs.cmake
    pkgs.gcc
    pkgs.gnumake
  ];
  text = ''
    set -eu

    cmake -S . -B build/local-tests -DCMAKE_BUILD_TYPE=Release -DCSTD_BUILD_BENCHMARKS=OFF
    cmake --build build/local-tests
    ctest --test-dir build/local-tests --output-on-failure
  '';
}
