# MIT License
#
# Copyright (c) 2026 Tommaso Bruno
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

function(vs_discover_tests)
  file(GLOB TEST_FILES RELATIVE "${PROJECT_SOURCE_DIR}" "tests/*/*_test.c" "tests/*/*/*_test.c")

  if(NOT TEST_FILES)
    message(WARNING "No tests found under tests/*/*_test.c or tests/*/*/*_test.c")
    return()
  endif()

  foreach(TEST_FILE_REL ${TEST_FILES})
    string(REPLACE "/" ";" TEST_PARTS "${TEST_FILE_REL}")
    list(LENGTH TEST_PARTS TEST_PARTS_LEN)
    if(NOT TEST_PARTS_LEN EQUAL 3 AND NOT TEST_PARTS_LEN EQUAL 4)
      message(FATAL_ERROR "Unsupported test layout: ${TEST_FILE_REL}")
    endif()

    list(GET TEST_PARTS 1 TARGET_GROUP)
    if(TEST_PARTS_LEN EQUAL 3)
      list(GET TEST_PARTS 2 TEST_FILENAME)
      string(REGEX REPLACE "_test\\.c$" "" TARGET_NAME "${TEST_FILENAME}")
      set(TEST_VARIANT "")
    else()
      list(GET TEST_PARTS 2 TARGET_NAME)
      list(GET TEST_PARTS 3 TEST_FILENAME)
      string(REGEX REPLACE "_test\\.c$" "" TEST_VARIANT "${TEST_FILENAME}")
    endif()

    if(TEST_VARIANT STREQUAL "")
      set(EXECUTABLE_NAME "${TARGET_GROUP}-${TARGET_NAME}")
      set(CTEST_NAME "${TARGET_GROUP}/${TARGET_NAME}")
    else()
      set(EXECUTABLE_NAME "${TARGET_GROUP}-${TARGET_NAME}-${TEST_VARIANT}")
      set(CTEST_NAME "${TARGET_GROUP}/${TARGET_NAME}/${TEST_VARIANT}")
    endif()
    add_executable(${EXECUTABLE_NAME} "${PROJECT_SOURCE_DIR}/${TEST_FILE_REL}")
    target_include_directories(${EXECUTABLE_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE vstd::vstd)

    add_test(NAME ${CTEST_NAME} COMMAND ${EXECUTABLE_NAME})
    set_tests_properties(${CTEST_NAME} PROPERTIES LABELS "${TARGET_GROUP};${TARGET_NAME}")
  endforeach()
endfunction()
