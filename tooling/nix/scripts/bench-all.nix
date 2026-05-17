{pkgs}:
pkgs.writeShellApplication {
  name = "bench-all";
  runtimeInputs = [
    pkgs.cmake
    pkgs.gcc
    pkgs.gnumake
  ];
  text = ''
    set -eu

    timestamp="$(date +%Y-%m-%d_%H%M%S)"
    full_sha="$(git rev-parse HEAD 2>/dev/null || printf '%s' nogit)"

    cmake -S . -B build/bench -DCMAKE_BUILD_TYPE=Release -DCSTD_BUILD_BENCHMARKS=ON
    cmake --build build/bench

    printf '%s\n' "benchmark_run_timestamp=$timestamp"
    printf '%s\n' "benchmark_commit_full=$full_sha"
    printf '%s\n' "benchmark_compiler=gcc $(gcc -dumpfullversion -dumpversion)"
    printf '%s\n' ""

    bench_found=0
    for bench_bin in build/bench/bench-*; do
      if [ -x "$bench_bin" ]; then
        bench_found=1
        "$bench_bin"
      fi
    done

    if [ "$bench_found" -ne 1 ]; then
      printf '%s\n' "No benchmark executables found under build/bench/bench-*" >&2
      exit 1
    fi
  '';
}
