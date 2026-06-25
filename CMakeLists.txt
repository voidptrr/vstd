cmake_minimum_required(VERSION 3.20)

project(vstd_ctests C)

include(CTest)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_program(ZIG_EXECUTABLE zig REQUIRED)

set(VSTD_ZIG_PREFIX "${CMAKE_BINARY_DIR}/zig-prefix")
set(VSTD_ZIG_CACHE "${CMAKE_BINARY_DIR}/zig-cache")
set(VSTD_ZIG_GLOBAL_CACHE "${CMAKE_BINARY_DIR}/zig-global-cache")
set(VSTD_ZIG_LIBRARY "${VSTD_ZIG_PREFIX}/lib/libvstd.a")
set(VSTD_CTEST_ZIG_OPTIMIZE "ReleaseFast" CACHE STRING "Zig optimization mode for CTest library")

file(GLOB_RECURSE VSTD_LIBRARY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_custom_command(
    OUTPUT "${VSTD_ZIG_LIBRARY}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory
            "${VSTD_ZIG_CACHE}" "${VSTD_ZIG_GLOBAL_CACHE}"
    COMMAND "${CMAKE_COMMAND}" -E env
            "ZIG_GLOBAL_CACHE_DIR=${VSTD_ZIG_GLOBAL_CACHE}"
            "${ZIG_EXECUTABLE}" build install
            --cache-dir "${VSTD_ZIG_CACHE}"
            "-Doptimize=${VSTD_CTEST_ZIG_OPTIMIZE}"
            --prefix "${VSTD_ZIG_PREFIX}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
        "${CMAKE_CURRENT_SOURCE_DIR}/build.zig"
        ${VSTD_LIBRARY_SOURCES}
    VERBATIM
)

add_custom_target(vstd_zig_library DEPENDS "${VSTD_ZIG_LIBRARY}")

add_library(vstd STATIC IMPORTED GLOBAL)
set_target_properties(vstd PROPERTIES IMPORTED_LOCATION "${VSTD_ZIG_LIBRARY}")
add_dependencies(vstd vstd_zig_library)

file(GLOB_RECURSE VSTD_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.c"
)
list(SORT VSTD_TEST_SOURCES)

add_custom_target(vstd_ctests)

foreach(test_source IN LISTS VSTD_TEST_SOURCES)
    file(RELATIVE_PATH test_relative "${CMAKE_CURRENT_SOURCE_DIR}/tests" "${test_source}")
    string(REGEX REPLACE "_test\\.c$" "" test_name "${test_relative}")
    string(REPLACE "/" "-" test_name "${test_name}")
    string(REPLACE "_" "-" test_name "${test_name}")

    add_executable("${test_name}" "${test_source}")
    target_include_directories("${test_name}" PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
    target_compile_options("${test_name}" PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
    target_link_libraries("${test_name}" PRIVATE vstd)
    add_dependencies(vstd_ctests "${test_name}")
    add_test(NAME "${test_name}" COMMAND "${test_name}")
endforeach()
