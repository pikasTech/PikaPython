# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

find_program(python_exec "python3")
if(python_exec)
    execute_process(
        COMMAND
        "${python_exec}" -c "import sys; sys.stdout.write('.'.join([str(x) for x in system.version_info[:2]]))"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE version
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
else()
    # python3 does not exist on Windows once Python 3.x is installed, but python does
    find_program(python_exec "python")
    if(python_exec)
        execute_process(
            COMMAND
            "${python_exec}" -c "import sys; sys.stdout.write('.'.join([str(x) for x in system.version_info[:2]]))"
            RESULT_VARIABLE result
            OUTPUT_VARIABLE version
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
    else()
        message(FATAL_ERROR "python can't be found in $ENV{PATH}")
    endif()
endif()
set(PYTHON_EXECUTABLE ${python_exec})
