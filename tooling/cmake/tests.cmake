function(cstd_discover_tests)
  file(GLOB TEST_FILES RELATIVE "${CMAKE_SOURCE_DIR}" "tests/*/*/*_test.c")

  if(NOT TEST_FILES)
    message(WARNING "No tests found under tests/*/*/*_test.c")
    return()
  endif()

  foreach(TEST_FILE_REL ${TEST_FILES})
    string(REPLACE "/" ";" TEST_PARTS "${TEST_FILE_REL}")
    list(LENGTH TEST_PARTS TEST_PARTS_LEN)
    if(NOT TEST_PARTS_LEN EQUAL 4)
      message(FATAL_ERROR "Unsupported test layout: ${TEST_FILE_REL}")
    endif()

    list(GET TEST_PARTS 1 TARGET_GROUP)
    list(GET TEST_PARTS 2 TARGET_NAME)
    list(GET TEST_PARTS 3 TEST_FILENAME)

    string(REGEX REPLACE "_test\\.c$" "" TEST_VARIANT "${TEST_FILENAME}")

    set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/src/${TARGET_GROUP}/${TARGET_NAME}.c")
    if(NOT EXISTS "${SOURCE_FILE}")
      message(FATAL_ERROR "Missing source file for ${TEST_FILE_REL}: ${SOURCE_FILE}")
    endif()

    set(EXTRA_SOURCES "")
    if(TARGET_GROUP STREQUAL "datastruct" AND TARGET_NAME STREQUAL "binary_heap")
      list(APPEND EXTRA_SOURCES "${CMAKE_SOURCE_DIR}/src/datastruct/vector.c")
    endif()

    set(EXECUTABLE_NAME "${TARGET_GROUP}-${TARGET_NAME}-${TEST_VARIANT}")
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE} ${EXTRA_SOURCES} "${CMAKE_SOURCE_DIR}/${TEST_FILE_REL}")
    target_include_directories(${EXECUTABLE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/src")

    set(CTEST_NAME "${TARGET_GROUP}/${TARGET_NAME}/${TEST_VARIANT}")
    add_test(NAME ${CTEST_NAME} COMMAND ${EXECUTABLE_NAME})
    set_tests_properties(${CTEST_NAME} PROPERTIES LABELS "${TARGET_GROUP};${TARGET_NAME}")
  endforeach()
endfunction()
