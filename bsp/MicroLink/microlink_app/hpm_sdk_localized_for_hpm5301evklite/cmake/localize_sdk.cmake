# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

set(result 0)
if(UNLOCALIZE_PROJECT)
    if(EXISTS "${SRC_DIR}/CMakeLists.txt.localized.bak")
        file(TO_CMAKE_PATH ${HPM_SDK_BASE}/scripts/localize_sdk.py LOCALIZE_PYTHON_SCRIPT)
        execute_process(
            COMMAND
            ${PYTHON_EXECUTABLE}
            ${LOCALIZE_PYTHON_SCRIPT}
            --gnt=${GENERATOR}
            --sdk=${HPM_SDK_BASE}
            --src=${SRC_DIR}
            --bld=${BIN_DIR}
            --dst=${DST_DIR}
            --brd=${BOARD}
            --unlocalize
            WORKING_DIRECTORY ${BIN_DIR}
            RESULT_VARIABLE result
        )
    else()
        message(WARNING "\nproject has not been localized yet, localize it first!")
    endif()
else()
    if(NOT EXISTS "${SRC_DIR}/CMakeLists.txt.localized.bak")
        file(TO_CMAKE_PATH ${HPM_SDK_BASE}/scripts/localize_sdk.py LOCALIZE_PYTHON_SCRIPT)
        execute_process(
            COMMAND
            ${PYTHON_EXECUTABLE}
            ${LOCALIZE_PYTHON_SCRIPT}
            --gnt=${GENERATOR}
            --sdk=${HPM_SDK_BASE}
            --src=${SRC_DIR}
            --bld=${BIN_DIR}
            --dst=${DST_DIR}
            --brd=${BOARD}
            -f
            WORKING_DIRECTORY ${BIN_DIR}
            RESULT_VARIABLE result
        )
    else()
        message(WARNING "\nproject has been localized already, unlocalize it first!")
    endif()
endif()

if(NOT ${result} EQUAL 0)
    message(FATAL_ERROR "\nLocalization failed, abort!")
endif()
