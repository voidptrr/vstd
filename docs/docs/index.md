# k4c

`k4c` is a practical utilities and data-structures kit for C projects.
It bundles reusable low-level building blocks with consistent behavior,
predictable error handling, and a compact API style that stays easy to adopt.

Core principles:

- Reusable utilities and container primitives for everyday C development.
- Common behavior contracts across data structures (null handling, empty handling, k4c_status returns).
- Small, practical API surface focused on clarity and portability.

## Platform

`k4c` is built for Linux services first. Implementations may use POSIX/Linux
system APIs directly when they are the simplest fit for service-oriented C
programs.

macOS support is best-effort for development workflows. Native Windows support
is not a target; use WSL, Docker, or a Linux deployment environment when running
software built on `k4c`.
