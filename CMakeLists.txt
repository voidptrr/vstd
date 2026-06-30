cmake_minimum_required(VERSION 3.20)

project(k4c VERSION 0.2.2 LANGUAGES C)

include(CTest)
include(GNUInstallDirs)
enable_testing()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB LIBRARY_ROOT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

file(GLOB_RECURSE LIBRARY_MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/allocators/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/allocators/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/buffer/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/buffer/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/datastruct/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/datastruct/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/hash/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/hash/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/io/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/io/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

set(LIBRARY_SOURCES
    ${LIBRARY_ROOT_SOURCES}
    ${LIBRARY_MODULE_SOURCES}
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
        "${CMAKE_CURRENT_SOURCE_DIR}"
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
