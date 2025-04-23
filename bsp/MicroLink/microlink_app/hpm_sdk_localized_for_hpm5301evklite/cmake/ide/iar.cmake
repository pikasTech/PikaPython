# Copyright 2023-2024 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

# add iar library interface to store iar specific configurations
set(HPM_SDK_IAR_LIB_ITF hpm_sdk_iar_lib_itf)
add_library(${HPM_SDK_IAR_LIB_ITF} INTERFACE)

set(HPM_SDK_IAR_LIB hpm_sdk_iar_lib)
define_property(GLOBAL PROPERTY ${HPM_SDK_IAR_LIB} BRIEF_DOCS "iar library" FULL_DOCS "iar library")

set(HPM_SDK_IAR_STARTUP_LIB hpm_sdk_iar_startup_lib)
define_property(GLOBAL PROPERTY ${HPM_SDK_IAR_STARTUP_LIB} BRIEF_DOCS "iar library" FULL_DOCS "iar library")

set(IAR_OPT_LIB_IO_TYPE LIBRARY_IO_TYPE)
set(IAR_OPT_DBG_TGT_CONN debug_target_connection)
set(IAR_OPT_LINKER_PRINTF_FP_ENABLED linker_printf_fp_enabled)
set(IAR_OPT_LINKER_SCANF_FP_ENABLED linker_scanf_fp_enabled)
set(IAR_OPT_LINKER_PRINTF_FMT_LEVEL linker_printf_fmt_level)
set(IAR_OPT_LINKER_SCANF_FMT_LEVEL linker_scanf_fmt_level)
set(IAR_OPT_DBG_JLINK_SPEED speed)
set(IAR_DBG_CONN_GDB_SRV "GDB Server")

set(HPM_SDK_IAR_OPTS hpm_sdk_iar_opts)
define_property(GLOBAL PROPERTY ${HPM_SDK_IAR_OPTS} BRIEF_DOCS "iar options" FULL_DOCS "iar options")

# Set compile options for IAR
#
# Example:
#   sdk_iar_compile_options(opts)
# :param opts: compile options
# @public
#
function(sdk_iar_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_IAR_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set compile definitions for IAR
#
# Example:
#   sdk_iar_compile_definitions(def)
# :param def: compiler preprocesing definition
# @public
#
function(sdk_iar_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_IAR_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

# link libraries for IAR
#
# Example:
#   sdk_iar_link_libraries(libs)
# :param libs: standard libraries to be linked for IAR
# @public
#
function(sdk_iar_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(${HPM_SDK_IAR_LIB_ITF} INTERFACE ${lib})
    endforeach()
endfunction()

# @private
#
function(sdk_iar_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_IAR_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# link libraries for IAR
#
# Example:
#   sdk_iar_ld_libs(libs)
# :param libs: libraries to be linked for IAR
# @private
#
function(sdk_iar_ld_lib)
    foreach(opt ${ARGN})
        set_property(TARGET ${HPM_SDK_IAR_LIB_ITF} APPEND PROPERTY INTERFACE_IAR_LD_INPUTS ${opt})
    endforeach()
endfunction()

# Add include path for IAR
#
# Example:
#   sdk_iar_link_libraries(libs)
# :param libs: libraries to be linked for IAR
# @public
#
function(sdk_iar_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(${HPM_SDK_IAR_LIB_ITF} INTERFACE ${path})
    endforeach()
endfunction()

# @private
#
function(sdk_iar_ld_lib_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_iar_ld_lib(${ARGN})
    endif()
endfunction()

# Add source file for IAR
#
# Example:
#   sdk_iar_src(SOURCE_FILE)
# :param SOURCE_FILE: source file added for IAR
# @public
#
function(sdk_iar_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_iar_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_IAR_LIB} ${path})
    endforeach()
endfunction()

# Add source file for IAR startup
#
# Example:
#   sdk_iar_startup_src(STARTUP_SOURCE_FILE)
# :param STARTUP_SOURCE_FILE: startup source file added for IAR
# @public
#
function(sdk_iar_startup_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_iar_startup_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_IAR_STARTUP_LIB} ${path})
    endforeach()
endfunction()


# Add source file (glob pattern) for IAR
#
# Example:
#   sdk_gcc_src_glob(SOURCE_FILE_GLOB)
# :param SOURCE_FILE_GLOB: source files to be added to IAR,
#    like ./**/*.c to add all .c files under current directory recursively
# @public
#
function(sdk_iar_src_glob)
    foreach(g ${ARGN})
        file(GLOB_RECURSE src ${g})
        list(APPEND globbed_src ${src})
    endforeach()
    sdk_iar_src(${globbed_src})
endfunction()

# Enable DSP in IAR
# @public
#
function(sdk_iar_enable_dsp)
    set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_IAR_OPTS} "enable_dsp=1")
endfunction()

# Set preinclude for IAR
#
# Example:
#   sdk_iar_preinclude(TYPE FILE)
# :param TYPE: different preinclude type, cc or asm
# :param FILE: different preinclude type, cc or asm
# @private
#
function(sdk_iar_preinclude type file)
    if(IS_DIRECTORY ${file})
        message(FATAL_ERROR "directory ${file} can't be added as preinclude")
    endif()
    if(IS_ABSOLUTE ${file})
        set(path ${file})
    else()
        set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    endif()
    set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_IAR_OPTS} "${type}_preinclude=${path}")
endfunction()

# Set cc preinclude for IAR
# @public
#
function(sdk_iar_cc_preinclude file)
    sdk_iar_preinclude("cc" ${file})
endfunction()

# Set asm preinclude for IAR
# @public
#
function(sdk_iar_asm_preinclude file)
    sdk_iar_preinclude("asm" ${file})
endfunction()

# Enable no size constraints
# @public
#
function(sdk_iar_enable_no_size_constraints)
    set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_IAR_OPTS} "iar_opt_no_size_const=1")
endfunction()

# Enable iar andes performance ext
# @public
#
function(sdk_iar_enable_andesperf)
    set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_IAR_OPTS} "enable_andesperf=1")
endfunction()

# Set IAR optioins
# @private
#
function(sdk_iar_options)
    foreach(opt ${ARGN})
        set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_IAR_OPTS} ${opt})
    endforeach()
endfunction()

function (generate_iar_project)
    get_property(target_source_files TARGET app PROPERTY SOURCES)
    get_property(target_app_include_dirs TARGET app PROPERTY INCLUDE_DIRECTORIES)
    get_property(target_lib_sources TARGET ${HPM_SDK_LIB} PROPERTY SOURCES)
    get_property(target_include_dirs TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    get_property(target_defines TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_COMPILE_DEFINITIONS)
    get_property(target_link_sym TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_LINK_SYMBOLS)
    get_property(target_gcc_cflags TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_COMPILE_OPTIONS)
    get_property(target_iar_cflags TARGET ${HPM_SDK_IAR_LIB_ITF} PROPERTY INTERFACE_COMPILE_OPTIONS)
    get_property(target_iar_ld_lib TARGET ${HPM_SDK_IAR_LIB_ITF} PROPERTY INTERFACE_IAR_LD_INPUTS)
    get_property(target_iar_source_files GLOBAL PROPERTY ${HPM_SDK_IAR_LIB})
    get_property(target_iar_startup_source_files GLOBAL PROPERTY ${HPM_SDK_IAR_STARTUP_LIB})
    get_property(target_iar_options GLOBAL PROPERTY ${HPM_SDK_IAR_OPTS})
    get_property(target_gcc_source_files TARGET ${HPM_SDK_GCC_LIB} PROPERTY SOURCES)
    get_property(target_iar_include_dirs TARGET ${HPM_SDK_IAR_LIB_ITF} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

    if(target_iar_ld_lib)
        foreach(f IN ITEMS ${target_iar_ld_lib})
            if("${target_iar_ld_input}" STREQUAL "")
                set(target_iar_ld_input ${f})
            else()
                set(target_iar_ld_input "${target_iar_ld_input};${f}")
            endif()
        endforeach()
    else()
        set(target_iar_ld_input "")
    endif()

    set(gcc_opt_level "")
    set(target_cflags "")
    set(target_source_files "${target_source_files};${target_iar_source_files}")

    foreach(f IN ITEMS ${target_iar_cflags})
        if(NOT target_cflags)
            set(target_cflags ${f})
        else()
            set(target_cflags "${target_cflags};${f}")
        endif()
    endforeach()

    foreach(f IN ITEMS ${target_gcc_cflags})
        string(FIND ${f} "-mabi=" exist)
        if(NOT ${exist} EQUAL -1)
            string(SUBSTRING ${f} 6 -1 IAR_COMPILER_ABI)
            continue()
        endif()
        string(FIND ${f} "-march=" exist)
        if(NOT ${exist} EQUAL -1)
            string(SUBSTRING ${f} 7 -1 IAR_COMPILER_ARCH)
            continue()
        endif()
        STRING(REGEX MATCH "-O[0-3s].*" exist ${f})
        if(NOT ${exist} EQUAL -1)
            STRING(REGEX REPLACE "(-O[0-3s])" "\\1" gcc_opt_level ${f})
            list(REMOVE_ITEM target_gcc_cflags ${f})
            continue()
        endif()
        STRING(REGEX MATCH "-D.*" exist ${f})
        if(NOT ${exist} EQUAL -1)
            if(NOT target_defines)
                set(target_defines ${f})
            else()
                set(target_defines "${target_defines};${f}")
            endif()
            list(REMOVE_ITEM target_gcc_cflags ${f})
            continue()
        endif()
    endforeach()

    # fallback option
    if(NOT IAR_COMPILER_ABI)
        set(IAR_COMPILER_ABI "ilp32")
    endif()
    if(NOT IAR_COMPILER_ARCH)
        set(IAR_COMPILER_ARCH ${RV_ARCH})
    endif()

    STRING(FIND ${IAR_COMPILER_ARCH} "_" exist)
    if(NOT ${exist} EQUAL -1)
        set(target_compiler_arch_exts_csr 0)
        set(target_compiler_arch_exts_fencei 0)
        # isa extenions will be processed according to given arch, user should be responsible for ext settings
        STRING(REGEX REPLACE "(rv[A-Za-z0-9]+)_([A-Za-z0-9_]+)" "\\1;\\2" RESULT ${IAR_COMPILER_ARCH})
        LIST(GET RESULT 0 IAR_COMPILER_ARCH)
        LIST(GET RESULT 1 IAR_COMPILER_ARCH_EXTS)
    else()
        # zicsr and zifencei will be set if no isa extension specified
        set(target_compiler_arch_exts_csr 1)
        set(target_compiler_arch_exts_fencei 1)
    endif()

    set(post_build_command "")
    set(is_secondary_core 0)
    if (DEFINED BUILD_FOR_SECONDARY_CORE)
        if (BUILD_FOR_SECONDARY_CORE)
            if (NOT DEFINED SEC_CORE_IMG_C_ARRAY_OUTPUT)
                set (SEC_CORE_IMG_C_ARRAY_OUTPUT "sec_core_img.c")
            endif()
            set(post_build_command "${PYTHON_EXECUTABLE} $(HPM_SDK_BASE)/scripts/bin2c.py $(OutDir)/demo.bin sec_core_img > ${SEC_CORE_IMG_C_ARRAY_OUTPUT}")
            string(REPLACE "\\" "/" post_build_command ${post_build_command})
            set(is_secondary_core 1)
        endif()
    endif()

    set(target_link_symbols "")
    foreach(sym IN ITEMS ${target_link_sym})
        string(FIND ${sym} "_heap_size" exist)
        if(NOT ${exist} EQUAL -1)
            continue()
        endif()
        string(FIND ${sym} "_stack_size" exist)
        if(NOT ${exist} EQUAL -1)
            continue()
        endif()
        if("${target_link_symbols}" STREQUAL "")
            set(target_link_symbols ${sym})
        else()
            set(target_link_symbols "${target_link_symbols},${sym}")
        endif()
    endforeach()

    set(target_sources "")
    foreach(src IN ITEMS ${target_source_files})
        if("${target_sources}" STREQUAL "")
            set(target_sources ${src})
        else()
            set(target_sources "${target_sources},${src}")
        endif()
    endforeach()

    get_property(target_linker_script TARGET ${APP_ELF_NAME} PROPERTY LINK_DEPENDS)
    if(CUSTOM_IAR_LINKER_FILE)
        set(target_linker ${CUSTOM_IAR_LINKER_FILE})
    else()
        # if no specific linker file is set, try to locate it
        string(REPLACE ".ld" ".icf" target_linker ${target_linker_script})
        string(REPLACE "/gcc/" "/iar/" target_linker ${target_linker})
    endif()

    # suppress warning
    set(iar_extra_options "--diag_suppress Pa089, --diag_suppress Pe236, --diag_suppress Pe188, --diag_suppress Pe546, --diag_suppress Pe111")
    set(enable_dsp "0")
    foreach(opt IN ITEMS ${target_iar_options})
        string(REPLACE "\"" "" opt ${opt}) # remove any quotes

        STRING(REGEX REPLACE "enable_dsp[space]*=[space]*([01])" "\\1" RESULT ${opt})
        if(NOT "${RESULT}" STREQUAL "${opt}")
            LIST(GET RESULT 0 enable_dsp)
            continue()
        endif()

        STRING(REGEX REPLACE "cc_preinclude[space]*=[space]*([a-zA-Z0-9/]*)" "\\1" RESULT ${opt})
        if(NOT "${RESULT}" STREQUAL "${opt}")
            LIST(GET RESULT 0 cc_preinclude)
            continue()
        endif()

        STRING(REGEX REPLACE "iar_opt_no_size_const[space]*=[space]*([01]*)" "\\1" RESULT ${opt})
        if(NOT "${RESULT}" STREQUAL "${opt}")
            LIST(GET RESULT 0 iar_opt_no_size_const)
            continue()
        endif()

        STRING(REGEX REPLACE "asm_preinclude[space]*=[space]*([a-zA-Z0-9/]*)" "\\1" RESULT ${opt})
        if(NOT "${RESULT}" STREQUAL "${opt}")
            LIST(GET RESULT 0 asm_preinclude)
            continue()
        endif()
        if("${iar_extra_options}" STREQUAL "")
            set(iar_extra_options "${opt}")
        else()
            set(iar_extra_options "${iar_extra_options},${opt}")
        endif()
    endforeach()

    set(enable_andesperf "0")
    if(${enable_dsp} STREQUAL "1")
        set(enable_andesperf "1")
    endif()

    if(NOT lib_io_type)
        set(lib_io_type STD)
    endif()

    if(NOT debug_target_conn)
        set(debug_target_conn ${IAR_DBG_CONN_GDB_SRV})
    endif()

    foreach (lib_src IN ITEMS ${target_lib_sources})
        set(target_sources "${target_sources},${lib_src}")
    endforeach ()

    foreach (lib_src IN ITEMS ${target_iar_source_files})
        set(target_sources "${target_sources},${lib_src}")
    endforeach ()

    foreach (lib_src IN ITEMS ${target_iar_startup_source_files})
        set(target_sources "${target_sources},${lib_src}")
    endforeach ()

    if (NOT IS_ABSOLUTE ${target_linker})
        set(target_linker ${CMAKE_CURRENT_SOURCE_DIR}/${target_linker})
    endif()

    if(EXISTS ${target_linker})
        message(STATUS "IAR linker script: " ${target_linker})
    else()
        message(WARNING "IAR: can not locate linker script: " ${target_linker})
        return()
    endif()

    # Remove duplicates
    list(REMOVE_DUPLICATES target_sources)
    list(REMOVE_DUPLICATES target_include_dirs)
    list(REMOVE_DUPLICATES target_iar_include_dirs)

    set(target_sources_with_macro "")
    foreach (target_source IN ITEMS ${target_sources})
        if(NOT IS_ABSOLUTE ${target_source})
            set(target_source ${CMAKE_CURRENT_SOURCE_DIR}/${target_source})
        endif()
        if("${target_source_with_macro}" STREQUAL "")
            set(target_sources_with_macro "${target_source}")
        else()
            set(target_sources_with_macro "${target_sources_with_macro},${target_source}")
        endif()
    endforeach ()

    list(REMOVE_DUPLICATES target_sources_with_macro)

    set(target_include_dirs_with_macro "")
    foreach (target_source IN ITEMS ${target_include_dirs} ${target_iar_include_dirs} ${target_app_include_dirs})
        string(FIND ${target_source} $ENV{GNURISCV_TOOLCHAIN_PATH} position)
        if(${position} EQUAL 0)
            continue()
        endif()
        if(NOT IS_ABSOLUTE ${target_source})
            set(target_source ${CMAKE_CURRENT_SOURCE_DIR}/${target_source})
        endif()
        if("${target_include_dirs_with_macro}" STREQUAL "")
            set(target_include_dirs_with_macro ${target_source})
        else()
            set(target_include_dirs_with_macro "${target_include_dirs_with_macro},${target_source}")
        endif()
    endforeach ()

    if(NOT "${target_defines}" STREQUAL "")
        set(target_defines_escaped "")
        foreach (defs IN ITEMS ${target_defines})
            string(REGEX REPLACE "\"" "\\\\\"" escaped ${defs})
            if("${target_defines_escaped}" STREQUAL "")
                set(target_defines_escaped ${escaped})
            else()
                set(target_defines_escaped "${target_defines_escaped},${escaped}")
            endif()
        endforeach()
    else()
        set(target_defines_escaped ${target_defines})
    endif()

    set(target_register_definition "${HPM_SDK_BASE}/soc/${HPM_SOC_SERIES}/${HPM_SOC}/hpm_iar_reg.xml")
    set(target_cpu_register_definition "${HPM_SDK_BASE}/soc/${HPM_SOC_SERIES}/${HPM_SOC}/hpm_iar_riscv_cpu_regs.xml")

    string(REPLACE "\\"  "/" target_sources_with_macro ${target_sources_with_macro})
    string(REPLACE "\\"  "/" target_include_dirs_with_macro ${target_include_dirs_with_macro})
    string(REPLACE "\\"  "/" target_linker ${target_linker})
    string(REPLACE "\\"  "/" target_register_definition ${target_register_definition})
    string(REPLACE "\\"  "/" target_cpu_register_definition ${target_cpu_register_definition})
    string(REPLACE "\\"  "/" hpm_sdk_base_path ${HPM_SDK_BASE})
    string(REPLACE "\\"  "/" hpm_board_path ${HPM_BOARD_DIR})

    if(IAR_COMPILER_ABI)
        set(target_compiler_abi ${IAR_COMPILER_ABI})
    else()
        set(target_compiler_abi "ilp32")
    endif()

    if(IAR_COMPILER_ARCH)
        set(target_compiler_arch ${IAR_COMPILER_ARCH})
    else()
        set(target_compiler_arch "rv32ima")
    endif()

    if(${enable_dsp})
        set(target_compiler_arch "${target_compiler_arch}_Xandesdsp")
    endif()

    if(${enable_andesperf})
        set(target_compiler_arch "${target_compiler_arch}_Xandesperf")
    endif()

    if(IAR_COMPILER_ARCH_EXTS)
        STRING(REPLACE "_" ";" RESULT ${IAR_COMPILER_ARCH_EXTS})
        FOREACH(i IN LISTS RESULT)
            if(${i} STREQUAL "zicsr")
                set(target_compiler_arch_exts_csr 1)
            endif()
            if(${i} STREQUAL "zifencei")
                set(target_compiler_arch_exts_fencei 1)
            endif()
            if(${i} STREQUAL "zba")
                set(target_compiler_arch "${target_compiler_arch}_Zba")
            endif()
            if(${i} STREQUAL "zbb")
                set(target_compiler_arch "${target_compiler_arch}_Zbb")
            endif()
            if(${i} STREQUAL "zbc")
                set(target_compiler_arch "${target_compiler_arch}_Zbc")
            endif()
            if(${i} STREQUAL "zbs")
                set(target_compiler_arch "${target_compiler_arch}_Zbs")
            endif()
        ENDFOREACH()
    endif()


    # Specify IAR target device name to be defined in IAR project file
    if (NOT HPM_DEVICE_NAME)
        set(IAR_DEVICE_NAME "${HPM_SOC}XXXX")
    else()
        set(IAR_DEVICE_NAME ${HPM_DEVICE_NAME})
    endif()

    if (${is_secondary_core})
        set(IAR_DEVICE_NAME "${HPM_DEVICE_NAME}_CPU1")
    endif()

    message(STATUS "IAR device name: ${IAR_DEVICE_NAME}")
    set(IAR_DEVICE_NAME "${IAR_DEVICE_NAME}\\tHPMicro ${IAR_DEVICE_NAME}")

    set(linkded_project_name "")
    set(linkded_project_path "")
    if (LINKED_PROJECT_NAME)
        set(linked_project_name ${LINKED_PROJECT_NAME})
    else()
        set(linkded_project_name "Untitled")
    endif()
    if (IS_ABSOLUTE ${LINKED_PROJECT_PATH})
        set(linked_project_path ${LINKED_PROJECT_PATH})
    else()
        set(linked_project_path "${CMAKE_CURRENT_SOURCE_DIR}/${LINKED_PROJECT_PATH}")
    endif()
    string(REPLACE "\\"  "/" linked_project_path ${linked_project_path})

    if (NOT EXISTS ${linked_project_path})
        message(WARNING "linked project ${linked_project_path} can not be located")
    endif()

    set(target_data "
        \"name\": \"${CMAKE_PROJECT_NAME}\",
        \"sources\": \"${target_sources_with_macro}\",
        \"includes\": \"${target_include_dirs_with_macro}\",
        \"defines\":\"${target_defines_escaped}\",
        \"linker\":\"${target_linker}\",
        \"link_symbols\":\"${target_link_symbols}\",
        \"sdk_base\":\"${hpm_sdk_base_path}\",
        \"board\":\"${BOARD}\",
        \"board_dir\":\"${hpm_board_path}\",
        \"soc\":\"${HPM_SOC}\",
        \"register_definition\":\"${target_register_definition}\",
        \"cpu_register_definition\":\"${target_cpu_register_definition}\",
        \"post_build_command\":\"${post_build_command}\",
        \"heap_size\":\"${HEAP_SIZE}\",
        \"stack_size\":\"${STACK_SIZE}\",
        \"cplusplus\":\"${CMAKE_CXX_STANDARD}\",
        \"gcc_opt_level\":\"${gcc_opt_level}\",
        \"target_device_name\":\"${IAR_DEVICE_NAME}\",
        \"cflags\":\"${target_cflags}\",
        \"compiler_abi\":\"${target_compiler_abi}\",
        \"compiler_arch\":\"${target_compiler_arch}\",
        \"compiler_arch_exts_csr\":\"${target_compiler_arch_exts_csr}\",
        \"compiler_arch_exts_fencei\":\"${target_compiler_arch_exts_fencei}\",
        \"iar_link_input\":\"${target_iar_ld_input}\",
        \"enable_dsp\":\"${enable_dsp}\",
        \"enable_andesperf\":\"${enable_andesperf}\",
        \"enable_cpp_exceptions\":\"${enable_cpp_exceptions}\",
        \"extra_iar_options\":\"${iar_extra_options}\",
        \"linked_project_name\":\"${linked_project_name}\",
        \"linked_project_path\":\"${linked_project_path}\",
        \"is_secondary_core\":\"${is_secondary_core}\",
        \"cc_preinclude\":\"${cc_preinclude}\",
        \"asm_preinclude\":\"${asm_preinclude}\",
        \"iar_opt_no_size_const\":\"${iar_opt_no_size_const}\"
        ")

    set(IAR_PROJECT_PATH "${CMAKE_CURRENT_BINARY_DIR}/iar_embedded_workbench")
    set(IAR_JSON_FILE_PATH "${IAR_PROJECT_PATH}/${CMAKE_PROJECT_NAME}.json")
    set(SDK_SCRIPT_IDE_PATH "${HPM_SDK_BASE}/scripts/ide")
    file(WRITE ${IAR_JSON_FILE_PATH}
        "{
            \"target\": {
                ${target_data}
            }
        }")

    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${SDK_SCRIPT_IDE_PATH}/iar/embedded_workbench_proj_gen.py "${IAR_JSON_FILE_PATH}" "${IAR_PROJECT_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}"
        WORKING_DIRECTORY  ${SDK_SCRIPT_IDE_PATH}/iar
        )
endfunction ()
