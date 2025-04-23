# Copyright (c) 2021-2022,2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE INTERNAL "" FORCE)

if(NOT DEFINED ENV{GNURISCV_TOOLCHAIN_PATH})
    message(FATAL_ERROR "GNURISCV_TOOLCHAIN_PATH is not set yet")
endif()

set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "application source directory")
set(APP_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "application binary directory")
set(__build_dir ${APP_BIN_DIR}/build_tmp)

if(NOT APP_NAME)
    set(APP_NAME demo)
endif()
set(APP_ELF_NAME ${APP_NAME}.elf)
set(APP_BIN_NAME ${APP_NAME}.bin)
set(APP_MAP_NAME ${APP_NAME}.map)
set(APP_ASM_NAME ${APP_NAME}.asm)

set(HPM_SDK_LIB hpm_sdk_lib)
set(HPM_SDK_LIB_ITF hpm_sdk_lib_itf)

# store all options
add_library(${HPM_SDK_LIB_ITF} INTERFACE)

add_library(app OBJECT "")

add_executable(${APP_ELF_NAME} $<TARGET_OBJECTS:app>)

message(STATUS "Application: ${APP_SRC_DIR}")

file(TO_CMAKE_PATH "${HPM_SDK_BASE}" PROJECT_SOURCE_DIR)

set(PROJECT_SOURCE_DIR ${HPM_SDK_BASE})
set(PROJECT_BINARY_DIR ${__build_dir})

set(LIBRARY_OUTPUT_PATH ${APP_BIN_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${APP_BIN_DIR}/output)

# search board in extra_board_path if given
if(BOARD_SEARCH_PATH AND EXISTS ${BOARD_SEARCH_PATH})
    find_path(BOARD_SEARCH_DIR NAMES ${BOARD}.yaml PATHS ${BOARD_SEARCH_PATH}/* NO_DEFAULT_PATH)
    if(BOARD_SEARCH_DIR)
        set(BOARD_MESSAGE "Board (custom board): ${BOARD} from ${BOARD_SEARCH_PATH}")
        set(HPM_BOARD_DIR ${BOARD_SEARCH_PATH}/${BOARD})
    endif()
endif()

# search board in sdk
if(NOT HPM_BOARD_DIR OR NOT EXISTS ${HPM_BOARD_DIR})
    find_path(SDK_BOARD_DIR NAMES ${BOARD}.yaml PATHS ${HPM_SDK_BASE}/boards/* NO_DEFAULT_PATH)
    if(SDK_BOARD_DIR)
        set(BOARD_MESSAGE "Board: ${BOARD} from ${HPM_SDK_BASE}/boards")
        set(HPM_BOARD_DIR ${HPM_SDK_BASE}/boards/${BOARD})
    endif()
endif()

set(BOARD_YAML ${HPM_BOARD_DIR}/${BOARD}.yaml)
if(NOT HPM_BOARD_DIR OR NOT EXISTS ${BOARD_YAML})
    message(FATAL_ERROR "No board named '${BOARD}' found")
endif()
message(STATUS "${BOARD_MESSAGE}")

find_path(APP_YAML_PATH NAMES app.yaml PATHS ${APP_SRC_DIR} NO_DEFAULT_PATH)

# add basic extentions
include(${HPM_SDK_BASE}/cmake/python.cmake)
include(${HPM_SDK_BASE}/cmake/cmake-ext.cmake)

if(APP_YAML_PATH)
    get_app_linked_project_name("${APP_YAML_PATH}/app.yaml" LINKED_PROJECT_NAME)
    get_app_linked_project_path("${APP_YAML_PATH}/app.yaml" LINKED_PROJECT_PATH)
    get_app_excluded_ides("${APP_YAML_PATH}/app.yaml" EXCLUDED_IDES)
endif()

# include more advanced extentions
include(${HPM_SDK_BASE}/cmake/toolchain.cmake)
include(${HPM_SDK_BASE}/cmake/ide.cmake)
include(${HPM_SDK_BASE}/cmake/extra_flags.cmake)
include(${HPM_SDK_BASE}/cmake/ccache.cmake)
include(${HPM_SDK_BASE}/cmake/version.cmake)

# distclean target
add_custom_target(
    distclean
    COMMAND ${CMAKE_COMMAND} -DBIN_DIR=${APP_BIN_DIR}
    -DSRC_DIR=${APP_SRC_DIR} -P ${HPM_SDK_BASE}/cmake/cleanup.cmake
)

if(NOT LOCALIZATION_DST)
    set(LOCALIZATION_DST ${APP_SRC_DIR})
endif()

if(NOT EXISTS ${LOCALIZATION_DST})
    message(FATAL_ERROR "localization dest folder: \"${LOCALIZATION_DST}\" can not be found")
endif()

if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(TOOLCHAIN_OBJCOPY_COMMAND ${CROSS_COMPILE}llvm-objcopy)
    set(TOOLCHAIN_OBJDUMP_COMMAND ${CROSS_COMPILE}llvm-objdump)
else()
    set(TOOLCHAIN_OBJCOPY_COMMAND ${CROSS_COMPILE}objcopy)
    set(TOOLCHAIN_OBJDUMP_COMMAND ${CROSS_COMPILE}objdump)
endif()

add_custom_command(
    TARGET ${APP_ELF_NAME}
    COMMAND "${TOOLCHAIN_OBJCOPY_COMMAND}" -O binary -S ${EXECUTABLE_OUTPUT_PATH}/${APP_ELF_NAME} ${EXECUTABLE_OUTPUT_PATH}/${APP_BIN_NAME}
)

add_custom_command(
    TARGET ${APP_ELF_NAME}
    COMMAND "${TOOLCHAIN_OBJDUMP_COMMAND}" -S -d ${EXECUTABLE_OUTPUT_PATH}/${APP_ELF_NAME} > ${EXECUTABLE_OUTPUT_PATH}/${APP_ASM_NAME}
)

if(APP_YAML_PATH)
    check_board_capability(${BOARD_YAML} "${APP_YAML_PATH}/app.yaml" result)
    if(${result} STREQUAL "1")
        message(FATAL_ERROR "${BOARD} can not support this sample")
    endif()
    if(${result} STREQUAL "2")
        message(FATAL_ERROR "Failed to build core1 project for this sample")
    endif()
    check_excluded_targets("${APP_YAML_PATH}/app.yaml" excluded_targets)
    # check if specific minimum SDK version is needed
    get_app_min_sdk_version("${APP_YAML_PATH}/app.yaml" app_min_sdk_version)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DREQUIRED_MINIMUM_SDK_VERSION=${app_min_sdk_version}
        -DHPM_SDK_BASE=${HPM_SDK_BASE}
        -DSDK_VERSION_MAJOR=${SDK_VERSION_MAJOR}
        -DSDK_VERSION_MINOR=${SDK_VERSION_MINOR}
        -DSDK_PATCHLEVEL=${SDK_PATCHLEVEL}
        -P ${HPM_SDK_BASE}/cmake/compare_target_sdk_version.cmake
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        RESULT_VARIABLE result
        ERROR_VARIABLE error_output
    )
    if(${result})
        message(FATAL_ERROR ${error_output})
    endif()
endif()

# default build type is set to debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "debug")
else()
    string(TOLOWER ${CMAKE_BUILD_TYPE} cmake_build_type_lower)
    string(REGEX MATCH "^debug|release" is_ram ${cmake_build_type_lower})
    if("${is_ram}" STREQUAL "${cmake_build_type_lower}")
        # default HPM_BUILD_TYPE is ram
        if(NOT HPM_BUILD_TYPE)
            set(HPM_BUILD_TYPE "ram")
        endif()
        set(CMAKE_BUILD_TYPE ${cmake_build_type_lower})
    else()
        if(HPM_BUILD_TYPE)
            message(FATAL_ERROR "\n!!! since HPM_BUILD_TYPE is given, only \"debug\" or \"release\" is allowed for CMAKE_BUILD_TYPE\n")
        endif()
        string(REGEX MATCH "^(.*)_(debug|release)$" match_result ${cmake_build_type_lower})
        # backwards compatibility
        if(match_result)
            set(CMAKE_BUILD_TYPE ${CMAKE_MATCH_2})
            if(NOT HPM_BUILD_TYPE)
                set(HPM_BUILD_TYPE ${CMAKE_MATCH_1})
            endif()
        else()
            if(NOT HPM_BUILD_TYPE)
                set(HPM_BUILD_TYPE ${cmake_build_type_lower})
            endif()
            set(CMAKE_BUILD_TYPE "debug")
        endif()
    endif()
endif()

# default hpm build type is set to ram
if(NOT HPM_BUILD_TYPE)
    set(HPM_BUILD_TYPE "ram")
endif()

string(TOLOWER ${HPM_BUILD_TYPE} HPM_BUILD_TYPE)
set(supported_hpm_build_types "ram|flash_xip|flash_sdram_xip|flash_uf2|sec_core_img")
string(REGEX MATCH "${supported_hpm_build_types}" is_valid_hpm_build_type ${HPM_BUILD_TYPE})
if(NOT is_valid_hpm_build_type)
    message(FATAL_ERROR "\n!!! invalid HPM_BUILD_TYPE: ${HPM_BUILD_TYPE}, supported types:\n    ${supported_hpm_build_types}\n")
endif()

# target of "localize_sdk"
add_custom_target(
    localize_sdk
    COMMAND ${CMAKE_COMMAND}
    -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
    -DHPM_SDK_BASE=${HPM_SDK_BASE}
    -DSRC_DIR=${APP_SRC_DIR}
    -DDST_DIR=${LOCALIZATION_DST}
    -DBIN_DIR=${APP_BIN_DIR}
    -DBUILD_TYPE=${HPM_BUILD_TYPE}
    -DGENERATOR=${CMAKE_GENERATOR}
    -DBOARD=${BOARD}
    -P ${HPM_SDK_BASE}/cmake/localize_sdk.cmake
)

add_custom_target(
    unlocalize_sdk
    COMMAND ${CMAKE_COMMAND}
    -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
    -DHPM_SDK_BASE=${HPM_SDK_BASE}
    -DSRC_DIR=${APP_SRC_DIR}
    -DBIN_DIR=${APP_BIN_DIR}
    -DGENERATOR=${CMAKE_GENERATOR}
    -DBOARD=${BOARD}
    -DUNLOCALIZE_PROJECT=1
    -P ${HPM_SDK_BASE}/cmake/localize_sdk.cmake
)

set(BUILD_TYPE_NAME "${HPM_BUILD_TYPE}_${CMAKE_BUILD_TYPE}")
if((excluded_targets) AND (NOT ${BUILD_TYPE_NAME} STREQUAL ""))
    foreach(t IN ITEMS  ${excluded_targets})
        string(COMPARE EQUAL ${BUILD_TYPE_NAME} ${t} result)
        if (${result} EQUAL 1)
            message(FATAL_ERROR "\n!!! target ${HPM_BUILD_TYPE}:${CMAKE_BUILD_TYPE} has been excluded for this example\n")
        endif()
    endforeach()
endif()

get_soc_name_of_board(${BOARD_YAML} soc_name)
set(HPM_SOC ${soc_name})

# get soc series name
# example: soc: HPM6750 in soc/HPM6700 folder, get HPM6700 as soc series name
set(HPM_SOC_DIR ${HPM_SDK_BASE}/soc)
file(GLOB SUBDIRECTORIES LIST_DIRECTORIES true ${HPM_SOC_DIR}/*)
foreach(SUBDIR ${SUBDIRECTORIES})
    if(IS_DIRECTORY ${SUBDIR}/${HPM_SOC})
        get_filename_component(SOC_SERIES_NAME ${SUBDIR} NAME)
        set(HPM_SOC_SERIES ${SOC_SERIES_NAME})
        break()
    endif()
endforeach()

if(NOT DEFINED HPM_SOC_SERIES)
    message(FATAL_ERROR "\n!!! not found HPM_SOC_SERIES name for soc: ${HPM_SOC}")
endif()


# device name
get_device_name_of_board(${BOARD_YAML} device_name)
set(HPM_DEVICE_NAME ${device_name})

get_flash_size_of_board(${BOARD_YAML} flash_size)
get_extram_size_of_board(${BOARD_YAML} extram_size)

string(TOLOWER ${HPM_BUILD_TYPE} build_type)
if(NOT extram_size)
    string(FIND ${build_type} "sdram" found)
    if(${found} GREATER_EQUAL 0)
        message(FATAL_ERROR "\n!!! target ${build_type} is not supported for ${BOARD}\n")
    endif()
endif()

if(NOT HEAP_SIZE)
    SET(HEAP_SIZE 0x4000)    #segger default heap size
endif()
sdk_linker_global_symbols("_heap_size=${HEAP_SIZE}")

if(NOT STACK_SIZE)
    SET(STACK_SIZE 0x4000)    #segger default stack size
endif()
sdk_linker_global_symbols("_stack_size=${STACK_SIZE}")

# skip compiler check
set(CMAKE_C_COMPILER_FORCED 1)
set(CMAKE_CXX_COMPILER_FORCED 1)

enable_language(C CXX ASM)

# deal with applicaiton specific configurations
if(CONFIG_HPM_MATH_DSP)
    set(CONFIG_HPM_ENABLE_DSP TRUE)
endif()
add_subdirectory(${HPM_SDK_BASE} ${__build_dir})

if (DEFINED GEN_SEC_CORE_IMG_C_ARRAY)
    if(NOT DEFINED SEC_CORE_IMG_C_ARRAY_OUTPUT)
        set(SEC_CORE_IMG_C_ARRAY_OUTPUT "sec_core_img.c")
    endif()
    generate_bin2c_array(${SEC_CORE_IMG_C_ARRAY_OUTPUT})
endif()

# link final executable
target_link_libraries(app PUBLIC ${HPM_SDK_LIB_ITF} ${HPM_SDK_GCC_LIB_ITF})
if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    target_link_libraries(app PUBLIC ${HPM_SDK_ZCC_LIB_ITF})
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
    target_link_libraries(app PUBLIC ${HPM_SDK_NDSGCC_LIB_ITF})
endif()

if(${APP_SRC_DIR} STREQUAL ${APP_BIN_DIR})
    message(FATAL_ERROR "source directory is the same with binary directory.\
    please specify a build directory, e.g. cmake -Bbuild -H.")
endif()

