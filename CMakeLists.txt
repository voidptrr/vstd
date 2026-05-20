cmake_minimum_required(VERSION 3.20)

project(algorithms_and_data_structures_c C)

include(CTest)
enable_testing()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

include(tools/cmake/tests.cmake)
ckit_discover_tests()
