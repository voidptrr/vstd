cmake_minimum_required(VERSION 3.20)

project(vstd VERSION 0.2.0 LANGUAGES C)

include(CTest)
include(GNUInstallDirs)
enable_testing()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE VSTD_LIBRARY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_library(vstd STATIC ${VSTD_LIBRARY_SOURCES})
add_library(vstd::vstd ALIAS vstd)

target_compile_features(vstd PUBLIC c_std_17)
target_include_directories(
    vstd
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_compile_options(vstd PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

set_target_properties(
    vstd
    PROPERTIES
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
        EXPORT_NAME vstd
)

include(cmake/install.cmake)

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
    target_link_libraries("${test_name}" PRIVATE vstd::vstd)
    add_dependencies(vstd_ctests "${test_name}")
    add_test(NAME "${test_name}" COMMAND "${test_name}")
endforeach()
