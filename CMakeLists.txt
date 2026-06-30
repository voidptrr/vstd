cmake_minimum_required(VERSION 3.20)

project(k4c VERSION 0.2.2 LANGUAGES C)

include(CTest)
include(GNUInstallDirs)
enable_testing()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE LIBRARY_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_library(k4c STATIC ${LIBRARY_SOURCES})
add_library(k4c::k4c ALIAS k4c)

target_compile_features(k4c PUBLIC c_std_17)
target_include_directories(
    k4c
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)
target_compile_options(k4c PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

set_target_properties(
    k4c
    PROPERTIES
        VERSION "${PROJECT_VERSION}"
        SOVERSION "${PROJECT_VERSION_MAJOR}"
        EXPORT_NAME k4c
)

include(cmake/install.cmake)
include(cmake/tests.cmake)
