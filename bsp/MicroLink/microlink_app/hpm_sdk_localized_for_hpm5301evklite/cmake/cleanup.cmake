# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

get_filename_component(BINARY_DIR ${BIN_DIR} REALPATH)
get_filename_component(SOURCE_DIR ${SRC_DIR} REALPATH)
string(FIND ${SRC_DIR} ${BIN_DIR} INDEX)
if (NOT INDEX EQUAL -1)
    message(FATAL_ERROR "Can not run distclean in in-source build folder")
endif()

file(GLOB contents ${BIN_DIR}/*)
foreach(f ${contents})
    if(EXISTS ${f})
        file(REMOVE_RECURSE ${f})
    endif()
endforeach()
