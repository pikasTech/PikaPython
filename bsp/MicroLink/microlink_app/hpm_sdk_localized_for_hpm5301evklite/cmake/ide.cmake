# Copyright 2023 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

if(EXCLUDED_IDES)
    foreach(ide IN ITEMS ${EXCLUDED_IDES})
        if(${ide} STREQUAL "iar")
            set(EXCLUDED_IDE_IAR 1)
        elseif(${ide} STREQUAL "ses")
            set(EXCLUDED_IDE_SES 1)
        endif()
    endforeach()
endif()

include(${HPM_SDK_BASE}/cmake/ide/iar.cmake)
include(${HPM_SDK_BASE}/cmake/ide/segger.cmake)

function (generate_ide_projects)
    if(NOT EXCLUDED_IDE_IAR OR NOT ${EXCLUDED_IDE_IAR})
        generate_iar_project()
    endif()
    if(NOT EXCLUDED_IDE_SES OR NOT ${EXCLUDED_IDE_SES})
        generate_ses_project()
    endif()
endfunction()
