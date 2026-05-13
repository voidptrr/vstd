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
    short_sha="$(git rev-parse --short HEAD 2>/dev/null || printf '%s' nogit)"
    full_sha="$(git rev-parse HEAD 2>/dev/null || printf '%s' nogit)"
    log_dir="benchmarks/results"
    log_file="$log_dir/''${timestamp}_''${short_sha}.log"

    mkdir -p "$log_dir"

    cmake -S . -B build/bench -DCMAKE_BUILD_TYPE=Release
    cmake --build build/bench

    {
      printf '%s\n' "benchmark_run_timestamp=$timestamp"
      printf '%s\n' "benchmark_commit_full=$full_sha"
      printf '%s\n' "benchmark_compiler=gcc $(gcc -dumpfullversion -dumpversion)"
      printf '%s\n' ""
      cmake --build build/bench --target bench
    } | tee "$log_file"

    printf '%s\n' "Saved benchmark log to $log_file"
  '';
}
