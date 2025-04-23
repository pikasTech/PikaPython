# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_policy(SET CMP0079 NEW)

# Add source file to sdk core (HPM_SDK_LIB)
#
# Example:
#   sdk_src(SOURCE_FILE)
# :param SOURCE_FILE: source files to be added to HPM_SDK_LIB
# @public
#
function(sdk_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_lib_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        target_sources(${HPM_SDK_LIB} PRIVATE ${path})
    endforeach()
endfunction()

# Add include path
#
# Example:
#   sdk_inc(INC_PATH)
# :param INC_PATH: add include path
# @public
#
function(sdk_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(${HPM_SDK_LIB_ITF} INTERFACE ${path})
    endforeach()
endfunction()

# Add system include path
#
# Example:
#   sdk_sys_inc(SYS_INC_PATH)
# :param SYS_INC_PATH: add system include path
# @public
#
function(sdk_sys_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(${HPM_SDK_LIB_ITF} SYSTEM INTERFACE ${path})
    endforeach()
endfunction()

# Return all compiler options, separated by single space
#
# Example:
#   sdk_get_copmile_options(opts)
# :param opts: all compile options configured for current project
# @public
#
function(sdk_get_compile_options opts)
    get_target_property(all_opts ${HPM_SDK_LIB_ITF} INTERFACE_COMPILE_OPTIONS)
    string(JOIN " " output_opts ${all_opts})
    set(${opts} ${output_opts} PARENT_SCOPE)
endfunction()

# Define global symbols for linker
#
# Example:
#   sdk_linker_global_symbols(syms)
# :param syms: symboles for linker
# @public
#
function(sdk_linker_global_symbols)
    foreach(sym ${ARGN})
        target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE "-Xlinker --defsym=${sym}")
        get_property(link_symbols TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_LINK_SYMBOLS)
        list(APPEND link_symbols "${sym}")
        set_target_properties(${HPM_SDK_LIB_ITF} PROPERTIES INTERFACE_LINK_SYMBOLS "${link_symbols}")
    endforeach()
endfunction()

# Set compile options
#
# Example:
#   sdk_compile_options(opts)
# :param opts: compile options
# @public
#
function(sdk_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set compile definitions
#
# Example:
#   sdk_compile_definitions(def)
# :param def: compiler preprocesing definition
# @public
#
function(sdk_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

# set compile definitions if feature is true
#
# Example:
#   sdk_compile_definitions(def)
# :param def: compiler preprocesing definition
# @public
#
function(sdk_compile_definitions_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_compile_definitions(${ARGN})
    endif()
endfunction()

# Link libraries
#
# Example:
#   sdk_link_libraries(libs)
# :param libs: libraries to be linked, support both file path
#   (like USER_LIB.a) and standard libraries provided by toolchain (like m)
# @public
#
function(sdk_link_libraries)
    foreach(l ${ARGN})
        string(REGEX MATCH "\.[a-zA-Z]+$" is_file_name ${l})
        if(NOT ${is_file_name} STREQUAL "")
            # given library is an actual file name
            if(IS_ABSOLUTE ${l})
                set(lib ${l})
            else()
                set(lib ${CMAKE_CURRENT_SOURCE_DIR}/${l})
            endif()
            if(NOT EXISTS ${lib})
                message(WARNING "library ${lib} can't be found")
            else()
                target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE ${lib})
            endif()
        else()
            target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE ${l})
        endif()
    endforeach()
endfunction()

# link libraries if feature is true
#
# Example:
#   sdk_link_libraries_ifdef(FEATUREA libs)
# :param FEATUREA: if FEATUREA is true, opts will be added for linker
# :param libs: libraries to be linked, support both file path
#   (like USER_LIB.a) and standard libraries provided by toolchain (like m)
# @public
#
function(sdk_link_libraries_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_link_libraries(${ARGN})
    endif()
endfunction()

# Set linker options
#
# Example:
#   sdk_ld_options(opts)
# :param opts: linker options
# @public
#
function(sdk_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set linker options if feature is true
#
# Example:
#   sdk_ld_options_ifdef(FEATUREA opts)
# :param FEATUREA: if FEATUREA is true, opts will be added for linker
# :param opts: linker options
# @public
#
function(sdk_ld_options_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_ld_options(${ARGN})
    endif()
endfunction()

# get board information from board yaml
# @private
function(get_board_info board_yaml info_type info_value)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_board_info.py
        ${board_yaml}
        ${info_type}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE out
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    if(${out} STREQUAL "not found")
        set(out "0")
    endif()
    set(${info_value} ${out} PARENT_SCOPE)
endfunction()

# get probe name for openocd from board yaml
# @private
function(get_openocd_probe_name_of_board board_yaml out)
    get_board_info(${board_yaml} openocd-probe r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# get soc name for openocd from board yaml
# @private
function(get_openocd_soc_name_of_board board_yaml out)
    get_board_info(${board_yaml} openocd-soc r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# get soc name from board yaml
# @private
function(get_soc_name_of_board board_yaml out)
    get_board_info(${board_yaml} soc r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# get device name from board yaml
# @private
function(get_device_name_of_board board_yaml out)
    get_board_info(${board_yaml} device r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# Add subdirectory if feature is true
#
# Example:
#   add_subdirectory_ifdef(FEATUREA paths)
# :param FEATUREA: if FEATUREA is defined, paths will be added
# :param paths: directories to be added
# @public
#
function(add_subdirectory_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        foreach(d ${ARGN})
            add_subdirectory(${d})
        endforeach()
    endif()
endfunction()

# Add include path if feature is true
# @public
#
function(sdk_inc_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_inc(${ARGN})
    endif()
endfunction()

# Add include path if feature is false
# @public
#
function(sdk_inc_ifndef feature)
    if((NOT DEFINED ${feature}) OR (${feature} EQUAL 0))
        sdk_inc(${ARGN})
    endif()
endfunction()

# Add source to SDK core if feature is true
# @public
#
function(sdk_src_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_src(${ARGN})
    endif()
endfunction()

# Add source to SDK core if feature is false
# @public
#
function(sdk_src_ifndef feature)
    if((NOT DEFINED ${feature}) OR (${feature} EQUAL 0))
        sdk_src(${ARGN})
    endif()
endfunction()

# Clear all compile options
# @public
#
function(sdk_clear_compile_options)
    set_target_properties(${HPM_SDK_LIB_ITF} PROPERTIES INTERFACE_COMPILE_OPTIONS "")
endfunction()

# Add application specific include path
#
# Example:
#   sdk_app_inc(INC_PATH)
# :param INC_PATH: add include path for application
# @public
#
function(sdk_app_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(app PUBLIC ${path})
    endforeach()
endfunction()

# Add application source
#
# Example:
#   sdk_app_src(SOURCE_FILE)
# :param SOURCE_FILE: source files to be added to application
# @public
#
function(sdk_app_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_app_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        target_sources(app PRIVATE ${path})
    endforeach()
endfunction()

# check board capability based on board yaml and app.yaml
# @private
#
function(check_board_capability board_yaml app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/check_board_cap.py
        ${board_yaml}
        ${app_yaml}
        RESULT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# get flash size from board yaml
# @private
#
function(get_flash_size_of_board board_yaml out)
    get_board_info(${board_yaml} flash_size r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# get on-board external ram size
# @private
#
function(get_extram_size_of_board board_yaml out)
    get_board_info(${board_yaml} extram_size r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# @private
#
function(get_ses_debug_auto_start_gdb_server app_yaml start)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_debug_info.py
        ${app_yaml}
        ses
        auto_start_gdb_server
        RESULT_VARIABLE result
        OUTPUT_VARIABLE auto_start
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${start} ${auto_start} PARENT_SCOPE)
endfunction()

# @private
#
function(get_ses_debug_gdb_server_port app_yaml port)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_debug_info.py
        ${app_yaml}
        ses
        gdb_server_port
        RESULT_VARIABLE result
        OUTPUT_VARIABLE p
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${port} ${p} PARENT_SCOPE)
endfunction()

# @private
#
function(get_ses_debug_gdb_server_reset_command app_yaml rst_cmd)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_debug_info.py
        ${app_yaml}
        ses
        gdb_server_reset_command
        RESULT_VARIABLE result
        OUTPUT_VARIABLE p
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${rst_cmd} ${p} PARENT_SCOPE)
endfunction()

# @private
#
function(check_excluded_targets app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/check_excluded_targets.py
        ${app_yaml}
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# set compile definitions for soc modules
# @private
#
function(import_soc_modules soc_module_list)
  file(
    STRINGS
    ${soc_module_list}
    MODULE_LIST
    REGEX "^HPMSOC_"
    ENCODING "UTF-8"
  )

  foreach (m ${MODULE_LIST})
    string(REGEX MATCH "[^=]+" MODULE_NAME ${m})
    string(REGEX MATCH "=(.+$)" CONFIG_VALUE ${m})
    set(CONFIG_VALUE ${CMAKE_MATCH_1})

    if("${CONFIG_VALUE}" MATCHES "^\"(.*)\"$")
      set(CONFIG_VALUE ${CMAKE_MATCH_1})
    endif()

    set("${MODULE_NAME}" "${CONFIG_VALUE}" PARENT_SCOPE)
    if(("${CONFIG_VALUE}" STREQUAL "y") OR ("${CONFIG_VALUE}" STREQUAL "Y") OR ("${CONFIG_VALUE}" STREQUAL "1"))
        sdk_compile_definitions("-D${MODULE_NAME}=${CONFIG_VALUE}")
    endif()
  endforeach()
endfunction()

# Raise fatal error if symbols given are all not set or set to 0 or n
#
# Example:
#   sdk_raise_fatal_error_if_all_invalid(FEATUREA FEATUREB)
# :param FEATUREA FEAUTREB: features to be checked
# @public
#
function(sdk_raise_fatal_error_if_all_invalid)
  foreach(feature ${ARGN})
    if(NOT (${feature} AND (NOT ${${feature}} EQUAL 0)))
      message(FATAL_ERROR "ERROR: ${feature} needs to be set in ${ARGN}")
    endif()
  endforeach()
endfunction()

# Raise fatal error if symbols given are all set or set to 1 or y
#
# Example:
#   sdk_raise_fatal_error_if_all_valid(FEATUREA FEATUREB)
# :param FEATUREA FEAUTREB: features to be checked
# @public
#
function(sdk_raise_fatal_error_if_all_valid)
  foreach(feature ${ARGN})
    if(${feature} AND (NOT ${${feature}} EQUAL 0))
      message(FATAL_ERROR "ERROR: ${feature} is not expected to be set in ${ARGN}")
    endif()
  endforeach()
endfunction()

# Raise fatal error if at least one of them given symbols is set or set to 1 or y
#
# Example:
#   sdk_raise_fatal_error_if_valid_at_least_one(FEATUREA FEATUREB)
# :param FEATUREA FEAUTREB: features to be checked
# @public
#
function(sdk_raise_fatal_error_if_valid_at_least_one)
    foreach(feature ${ARGN})
        if((${feature}) AND (NOT ${${feature}} EQUAL 0))
            return()
        endif()
    endforeach()
    message(FATAL_ERROR "ERROR: at least one of ${ARGN} needs to set")
endfunction()

#### NDSGCC sections ####

# Set compile options for nds toolchain
#
# Example:
#   sdk_nds_compile_options(opts)
# :param opts: options for nds toolchain
# @public
#
function(sdk_nds_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set defnitions for nds toolchain
#
# Example:
#   sdk_nds_compie_definitions(defs)
# :param defs: definitions for andes toolchain
# @public
#
function(sdk_nds_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

# Link libraries for andes toolchains
#
# Example:
#   sdk_nds_link_libraries(libs)
# :param libs: libraries to be linked, support both file path
#    (like USER_LIB.a) and standard libraries provided by toolchain (like m)
# @public
function(sdk_nds_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${lib})
    endforeach()
endfunction()

# Set linker options
#
# Example:
#   sdk_nds_ld_options(opts)
# :param opts: linker options
# @public
#
function(sdk_nds_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

#### ZCC sections ####

# Set compile options for zcc toolchain
#
# Example:
#   sdk_zcc_compile_options(opts)
# :param opts: options for zcc toolchain
# @public
#
function(sdk_zcc_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_ZCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set defnitions for zcc toolchain
#
# Example:
#   sdk_zcc_compile_definitions(defs)
# :param defs: definitions for zcc toolchain
# @public
#
function(sdk_zcc_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_ZCC_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

# Link libraries for zcc toolchains
#
# Example:
#   sdk_zcc_link_libraries(libs)
# :param libs: libraries to be linked, support both file path
#    (like USER_LIB.a) and standard libraries provided by toolchain (like m)
# @public
function(sdk_zcc_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(${HPM_SDK_ZCC_LIB_ITF} INTERFACE ${lib})
    endforeach()
endfunction()

# Set linker options
#
# Example:
#   sdk_zcc_ld_options(opts)
# :param opts: linker options
# @public
#
function(sdk_zcc_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_ZCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Add directory if feature is false
function(add_subdirectory_ifndef feature dir)
    if(NOT DEFINED ${feature} OR "${${feature}}" EQUAL "0")
        add_subdirectory(${dir})
    endif()
endfunction()

# Add include path for gcc
#
# Example:
#   sdk_gcc_inc(INC_PATH)
# :param INC_PATH: add include path
# @public
#
function(sdk_gcc_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(${HPM_SDK_GCC_LIB_ITF} INTERFACE ${path})
    endforeach()
endfunction()

# Add source specifically for gcc
#
# Example:
#   sdk_gcc_src(SOURCE_FILE)
# :param SOURCE_FILE: source files to be added to HPM_SDK_LIB
# @public
#
function(sdk_gcc_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_lib_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        target_sources(${HPM_SDK_GCC_LIB} PRIVATE ${path})
    endforeach()
endfunction()

# Add source specifically for gcc startup
#
# Example:
#   sdk_gcc_startup_src(SOURCE_FILE)
# :param SOURCE_FILE: source files to be added to HPM_SDK_GCC_STARTUP_LIB
# @public
#
function(sdk_gcc_startup_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_lib_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        target_sources(${HPM_SDK_GCC_STARTUP_LIB} PRIVATE ${path})
    endforeach()
endfunction()


# get minimal SDK version needed by application
# @private
#
function(get_app_min_sdk_version app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_min_sdk_version.py
        ${app_yaml}
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# @private
#
function(get_app_linked_project_path app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_info.py
        ${app_yaml}
        "linked_project_path"
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# @private
#
function(get_app_linked_project_name app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_info.py
        ${app_yaml}
        "linked_project_name"
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# @private
#
function(get_app_excluded_ides app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_info.py
        ${app_yaml}
        "excluded_ides"
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# Add source file (glob pattern) to sdk core (HPM_SDK_LIB)
#
# Example:
#   sdk_src_glob(SOURCE_FILE_GLOB)
# :param SOURCE_FILE_GLOB: source files to be added to HPM_SDK_LIB,
#    like ./**/*.c to add all .c files under current directory recursively
# @public
#
function(sdk_src_glob)
    foreach(g ${ARGN})
        file(GLOB_RECURSE src ${g})
        list(APPEND globbed_src ${src})
    endforeach()
    sdk_src(${globbed_src})
endfunction()

# Add application source file (glob pattern)
#
# Example:
#   sdk_app_src_glob(SOURCE_FILE_GLOB)
# :param SOURCE_FILE_GLOB: source files to be added to application,
#    like ./**/*.c to add all .c files under current directory recursively
# @public
#
function(sdk_app_src_glob)
    foreach(g ${ARGN})
        file(GLOB_RECURSE src ${g})
        list(APPEND globbed_src ${src})
    endforeach()
    sdk_app_src(${globbed_src})
endfunction()

# Add source file (glob pattern) for gcc
#
# Example:
#   sdk_gcc_src_glob(SOURCE_FILE_GLOB)
# :param SOURCE_FILE_GLOB: source files to be added to gcc,
#    like ./**/*.c to add all .c files under current directory recursively
# @public
#
function(sdk_gcc_src_glob)
    foreach(g ${ARGN})
        file(GLOB_RECURSE src ${g})
        list(APPEND globbed_src ${src})
    endforeach()
    sdk_gcc_src(${globbed_src})
endfunction()

# Add source file (glob pattern) for gcc if feature is true
# @public
function(sdk_src_glob_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_src_glob(${ARGN})
    endif()
endfunction()

# Add source file (glob pattern) for gcc if feature is false
# @public
function(sdk_src_glob_ifndef feature)
    if((NOT DEFINED ${feature}) OR (${feature} EQUAL 0))
        sdk_src_glob(${ARGN})
    endif()
endfunction()

