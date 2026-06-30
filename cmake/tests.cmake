file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.c"
)
list(SORT TEST_SOURCES)

add_custom_target(k4c_ctests)

foreach(test_source IN LISTS TEST_SOURCES)
    file(RELATIVE_PATH test_relative "${CMAKE_CURRENT_SOURCE_DIR}/tests" "${test_source}")
    string(REGEX REPLACE "_test\\.c$" "" test_name "${test_relative}")
    string(REPLACE "/" "-" test_name "${test_name}")
    string(REPLACE "_" "-" test_name "${test_name}")

    add_executable("${test_name}" "${test_source}")
    target_include_directories("${test_name}" PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_compile_options("${test_name}" PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
    target_link_libraries("${test_name}" PRIVATE k4c::k4c)
    add_dependencies(k4c_ctests "${test_name}")
    add_test(NAME "${test_name}" COMMAND "${test_name}")
endforeach()
