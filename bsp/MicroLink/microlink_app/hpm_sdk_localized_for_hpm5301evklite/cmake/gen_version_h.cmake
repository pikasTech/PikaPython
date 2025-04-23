# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.0)

if(NOT DEFINED BUILD_VERSION)
  find_package(Git QUIET)
  if(GIT_FOUND AND EXISTS ${HPM_SDK_BASE}/.git)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe --abbrev=12 --always
      WORKING_DIRECTORY                ${HPM_SDK_BASE}
      OUTPUT_VARIABLE                  BUILD_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_STRIP_TRAILING_WHITESPACE
      ERROR_VARIABLE                   stderr
      RESULT_VARIABLE                  return_code
    )
    if(return_code)
      message(STATUS "git describe failed: ${stderr}")
    elseif(NOT "${stderr}" STREQUAL "")
      message(STATUS "git describe warned: ${stderr}")
    endif()
  endif()
endif()

configure_file(${HPM_SDK_BASE}/hpm_sdk_version.h.in ${OUT_FILE})
