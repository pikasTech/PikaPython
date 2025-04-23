# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.0)

include(${HPM_SDK_BASE}/cmake/hex.cmake)

if(REQUIRED_MINIMUM_SDK_VERSION)
    set(min_major 0)
    set(min_minor 0)
    set(min_patch 0)
    string(REGEX MATCH "^([0-9]*)\.([0-9]*)\.([0-9]*)" out ${REQUIRED_MINIMUM_SDK_VERSION})
    if(out)
        set(min_major ${CMAKE_MATCH_1})
        set(min_minor ${CMAKE_MATCH_2})
        set(min_patch ${CMAKE_MATCH_3})
    endif()
    if((${min_major} STRGREATER ${SDK_VERSION_MAJOR}) OR (${min_minor} STRGREATER ${SDK_VERSION_MINOR}) OR (${min_patch} STRGREATER ${SDK_PATCHLEVEL}))
        message(FATAL_ERROR "Current SDK (${SDK_VERSION_MAJOR}.${SDK_VERSION_MINOR}.${SDK_PATCHLEVEL}) is not supported, the required minimum SDK version for this application is ${min_major}.${min_minor}.${min_patch}")
    endif()
    unset(min_major)
    unset(min_minor)
    unset(min_patch)
endif()
