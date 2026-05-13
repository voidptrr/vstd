function(dsa_add_test TARGET_KEY TEST_FILE)
  set(options)
  set(oneValueArgs EXEC_SUFFIX TEST_SUITE TEST_NAME)
  set(multiValueArgs)
  cmake_parse_arguments(DSA_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  string(REPLACE "/" ";" TARGET_PARTS "${TARGET_KEY}")
  list(LENGTH TARGET_PARTS TARGET_PARTS_LEN)
  if(NOT TARGET_PARTS_LEN EQUAL 2)
    message(FATAL_ERROR "TARGET must be in format <group>/<name>, got: ${TARGET_KEY}")
  endif()

  list(GET TARGET_PARTS 0 TARGET_GROUP)
  list(GET TARGET_PARTS 1 TARGET_NAME_RAW)

  set(SOURCE_FILE "${CMAKE_SOURCE_DIR}/src/${TARGET_GROUP}/${TARGET_NAME_RAW}.c")
  if(NOT EXISTS "${SOURCE_FILE}")
    message(FATAL_ERROR "Missing source file for ${TARGET_KEY}: ${SOURCE_FILE}")
  endif()

  string(REPLACE "/" "-" EXECUTABLE_NAME "${TARGET_KEY}")
  if(DEFINED DSA_TEST_EXEC_SUFFIX AND NOT DSA_TEST_EXEC_SUFFIX STREQUAL "")
    set(EXECUTABLE_NAME "${EXECUTABLE_NAME}-${DSA_TEST_EXEC_SUFFIX}")
  endif()

  add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE} ${TEST_FILE})
  target_include_directories(${EXECUTABLE_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/src")

  set(CTEST_NAME "${EXECUTABLE_NAME}")
  if(DEFINED DSA_TEST_TEST_NAME AND NOT DSA_TEST_TEST_NAME STREQUAL "")
    set(CTEST_NAME "${DSA_TEST_TEST_NAME}")
  endif()
  add_test(NAME ${CTEST_NAME} COMMAND ${EXECUTABLE_NAME})

  set(TEST_LABELS "${TARGET_GROUP}")
  if(DEFINED DSA_TEST_TEST_SUITE AND NOT DSA_TEST_TEST_SUITE STREQUAL "")
    list(APPEND TEST_LABELS "${DSA_TEST_TEST_SUITE}")
  else()
    list(APPEND TEST_LABELS "${TARGET_NAME_RAW}")
  endif()
  set_tests_properties(${CTEST_NAME} PROPERTIES LABELS "${TEST_LABELS}")
endfunction()

function(dsa_discover_tests)
  file(GLOB TEST_FILES_FLAT RELATIVE "${CMAKE_SOURCE_DIR}" "tests/*/*_test.c")
  file(GLOB TEST_FILES_NESTED RELATIVE "${CMAKE_SOURCE_DIR}" "tests/*/*/*_test.c")
  set(TEST_FILES ${TEST_FILES_FLAT} ${TEST_FILES_NESTED})

  if(NOT TEST_FILES)
    message(WARNING "No tests found under tests/*/*_test.c or tests/*/*/*_test.c")
    return()
  endif()

  foreach(TEST_FILE_REL ${TEST_FILES})
    get_filename_component(TEST_DIR "${TEST_FILE_REL}" DIRECTORY)
    string(REPLACE "/" ";" TEST_DIR_PARTS "${TEST_DIR}")
    list(LENGTH TEST_DIR_PARTS TEST_DIR_PARTS_LEN)

    if(TEST_DIR_PARTS_LEN EQUAL 2)
      get_filename_component(TARGET_GROUP "${TEST_DIR}" NAME)
      get_filename_component(TEST_BASENAME "${TEST_FILE_REL}" NAME_WE)
      string(REGEX REPLACE "_test$" "" TARGET_NAME_RAW "${TEST_BASENAME}")

      set(TARGET_KEY "${TARGET_GROUP}/${TARGET_NAME_RAW}")
      dsa_add_test(
        "${TARGET_KEY}"
        "${CMAKE_SOURCE_DIR}/${TEST_FILE_REL}"
        TEST_NAME "${TARGET_KEY}"
      )
    elseif(TEST_DIR_PARTS_LEN EQUAL 3)
      list(GET TEST_DIR_PARTS 1 TARGET_GROUP)
      list(GET TEST_DIR_PARTS 2 TARGET_NAME_RAW)
      get_filename_component(TEST_BASENAME "${TEST_FILE_REL}" NAME_WE)
      string(REGEX REPLACE "_test$" "" TEST_VARIANT "${TEST_BASENAME}")

      set(TARGET_KEY "${TARGET_GROUP}/${TARGET_NAME_RAW}")
      dsa_add_test(
        "${TARGET_KEY}"
        "${CMAKE_SOURCE_DIR}/${TEST_FILE_REL}"
        EXEC_SUFFIX "${TEST_VARIANT}"
        TEST_SUITE "${TARGET_NAME_RAW}"
        TEST_NAME "${TARGET_KEY}/${TEST_VARIANT}"
      )
    else()
      message(FATAL_ERROR "Unsupported test layout: ${TEST_FILE_REL}")
    endif()
  endforeach()
endfunction()
