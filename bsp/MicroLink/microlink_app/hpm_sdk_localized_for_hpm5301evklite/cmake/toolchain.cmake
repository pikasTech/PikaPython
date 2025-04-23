# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

function(get_compiler_version compiler version_text compiler_version)
    if("${compiler}" STREQUAL "gcc")
        execute_process(
            COMMAND
            ${PYTHON_EXECUTABLE}
            ${HPM_SDK_BASE}/scripts/get_gcc_version.py
            "${version_text}"
            RESULT_VARIABLE result
            OUTPUT_VARIABLE v
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        set(${compiler_version} ${v} PARENT_SCOPE)
    elseif("${compiler}" STREQUAL "clang")
        execute_process(
            COMMAND
            ${PYTHON_EXECUTABLE}
            ${HPM_SDK_BASE}/scripts/get_gcc_version.py
            "${version_text}"
            RESULT_VARIABLE result
            OUTPUT_VARIABLE v
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        set(${compiler_version} ${v} PARENT_SCOPE)
    elseif("${compiler}" STREQUAL "zcc")
        execute_process(
            COMMAND
            ${PYTHON_EXECUTABLE}
            ${HPM_SDK_BASE}/scripts/get_gcc_version.py
            "${version_text}"
            RESULT_VARIABLE result
            OUTPUT_VARIABLE v
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        set(${compiler_version} ${v} PARENT_SCOPE)

    else()
        message(FATAL_ERROR "Unsupported compiler ${compiler}")
    endif()
endfunction()

set(HPM_SDK_NDSGCC_LIB_ITF hpm_sdk_ndsgcc_lib_itf)
add_library(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE)

set(HPM_SDK_ZCC_LIB_ITF hpm_sdk_zcc_lib_itf)
add_library(${HPM_SDK_ZCC_LIB_ITF} INTERFACE)

set(HPM_SDK_GCC_LIB_ITF hpm_sdk_gcc_lib_itf)
set(HPM_SDK_GCC_LIB hpm_sdk_gcc_lib)
set(HPM_SDK_GCC_STARTUP_LIB hpm_sdk_gcc_startup_lib)
add_library(${HPM_SDK_GCC_LIB_ITF} INTERFACE)

string(REGEX REPLACE "{\"\'}" "" GNURISCV_TOOLCHAIN_PATH $ENV{GNURISCV_TOOLCHAIN_PATH})

if(NOT EXISTS ${GNURISCV_TOOLCHAIN_PATH})
  message(FATAL_ERROR "Nothing found at GNURISCV_TOOLCHAIN_PATH: '${GNURISCV_TOOLCHAIN_PATH}'")
endif()

set(TOOLCHAIN_HOME ${GNURISCV_TOOLCHAIN_PATH})

set(TOOLCHAIN_VARIANT gcc)
if(NOT "$ENV{HPM_SDK_TOOLCHAIN_VARIANT}" STREQUAL "")
  set(TOOLCHAIN_VARIANT $ENV{HPM_SDK_TOOLCHAIN_VARIANT})
endif()

if(CUSTOM_TARGET_TRIPLET)
    set(TARGET_TRIPLET ${CUSTOM_TARGET_TRIPLET})
else()
    set(TARGET_TRIPLET riscv32-unknown-elf)
endif()

# determine which toolchain will be used
if("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
  set(COMPILER gcc)
  set(LINKER ld)
  set(BINTOOLS gnu)
  set(C++ g++)
  set(CROSS_COMPILE_TARGET riscv32-elf)
  set(SYSROOT_TARGET       riscv32-elf)
  set(TOOLCHAIN_CMAKE gcc.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
  set(COMPILER zcc)
  set(LINKER ld.lld)
  set(BINTOOLS gnu)
  set(C++ z++)
  set(SYSROOT_TARGET       riscv32-unknown-elf)
  set(TOOLCHAIN_CMAKE zcc.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "gcc")
  set(COMPILER gcc)
  set(LINKER ld)
  set(BINTOOLS gnu)
  set(C++ g++)
  set(CROSS_COMPILE_TARGET ${TARGET_TRIPLET})
  set(SYSROOT_TARGET       ${TARGET_TRIPLET})
  set(TOOLCHAIN_CMAKE gcc.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "nds-llvm")
  set(COMPILER clang)
  set(LINKER ld)
  set(BINTOOLS gnu)
  set(C++ clang++)
  set(CROSS_COMPILE_TARGET riscv32-elf)
  set(SYSROOT_TARGET       riscv32-elf)
  set(TOOLCHAIN_CMAKE llvm.cmake)
else()
  message(FATAL_ERROR "${TOOLCHAIN_VARIANT} is not supported")
endif()

if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
  set(CROSS_COMPILE ${TOOLCHAIN_HOME}/bin/)
else()
  set(CROSS_COMPILE ${TOOLCHAIN_HOME}/bin/${CROSS_COMPILE_TARGET}-)
endif()

message(STATUS "Found toolchain: gnu (${GNURISCV_TOOLCHAIN_PATH})")

set(CMAKE_SYSTEM_NAME Generic)
unset(CMAKE_C_COMPILER)
unset(CMAKE_CXX_COMPILER)
set(CC ${COMPILER})

find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}${CC}   PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)

if(CMAKE_C_COMPILER STREQUAL CMAKE_C_COMPILER-NOTFOUND)
    message(FATAL_ERROR "It was unable to find the toolchain. Is the environment misconfigured?
User-configuration:
GNURISCV_TOOLCHAIN_PATH: ${GNURISCV_TOOLCHAIN_PATH}
Internal variables:
CROSS_COMPILE: ${CROSS_COMPILE}
TOOLCHAIN_HOME: ${TOOLCHAIN_HOME}
")
endif()

execute_process(
  COMMAND ${CMAKE_C_COMPILER} --version
  RESULT_VARIABLE ret
  OUTPUT_VARIABLE version_text
  ERROR_QUIET
  )
if(ret)
  message(FATAL_ERROR "Executing the below command failed. Are permissions set correctly?
'${CMAKE_C_COMPILER} --version'
"
    )
endif()

# @private
function(get_nds_gcc_abi nds_gcc_abi)
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} --verbose
        RESULT_VARIABLE ret
        ERROR_VARIABLE verbose_text
        OUTPUT_QUIET
    )
    STRING(REGEX REPLACE ".*--with-abi=([A-Za-z0-9]+).*" "\\1" out ${verbose_text})
    set(${nds_gcc_abi} ${out} PARENT_SCOPE)
endfunction()

# Detect the ABI for andes toolchain explicitly
if(${TOOLCHAIN_VARIANT} STREQUAL "nds-gcc")
    get_nds_gcc_abi(nds_gcc_abi)
endif()

if(NOT RV_ABI)
    if(nds_gcc_abi)
        set(RV_ABI ${nds_gcc_abi})
    else()
        set(RV_ABI "ilp32")
    endif()
endif()

if(nds_gcc_abi)
    if(NOT ${RV_ABI} STREQUAL ${nds_gcc_abi})
        message(FATAL_ERROR "Specified RV_ABI: ${RV_ABI} does not match abi provided by nds_gcc ${nds_gcc_abi}")
    endif()
endif()

if(NOT RV_ARCH)
    set(RV_ARCH "rv32imac")
endif()

# @private
function(get_toolchain_gcc_spec spec)
    get_compiler_version(${COMPILER} "${version_text}" ver)
    set(COMPILER_VERSION ${ver})

    execute_process(
      COMMAND ${CMAKE_C_COMPILER} --verbose
      RESULT_VARIABLE ret
      ERROR_VARIABLE verbose_text
      OUTPUT_QUIET
      )
    STRING(REGEX REPLACE ".*--with-isa-spec=([A-Za-z0-9]+).*" "\\1" out ${verbose_text})
    set(${spec} ${out} PARENT_SCOPE)
endfunction()

get_toolchain_gcc_spec(spec)
if(spec GREATER_EQUAL 20191213)
    if(NOT "${RV_ARCH}" MATCHES "_")
        set(need_csr ON)
        set(need_fencei ON)
        STRING(FIND ${RV_ARCH} "g" exist)
        if("${RV_ARCH}" MATCHES "g")
            set(need_csr OFF)
            set(need_fencei OFF)
        elseif("${RV_ARCH}" MATCHES "[fd]")
            set(need_csr OFF)
        endif()
        # append isa ext
        if(need_csr AND NOT HPM_SDK_GCC_ISA_SPEC_NO_CSR)
            set(RV_ARCH "${RV_ARCH}_zicsr")
        endif()

        if(need_fencei AND NOT HPM_SDK_GCC_ISA_SPEC_NO_FENCEI)
            set(RV_ARCH "${RV_ARCH}_zifencei")
        endif()
    endif()
endif()

# remove timestamp from libraries
set(CMAKE_ASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <LINK_FLAGS> <OBJECTS>")

set(CMAKE_CXX_STANDARD 11)
set(CXX ${C++})
find_program(CMAKE_CXX_COMPILER ${CROSS_COMPILE}${CXX}   PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)
# include toolchain specific settings
include(${HPM_SDK_BASE}/cmake/toolchain/${TOOLCHAIN_CMAKE})

function (generate_bin2c_array c_array_path)
    add_custom_command(
        TARGET ${APP_ELF_NAME}
        COMMAND ${PYTHON_EXECUTABLE} $ENV{HPM_SDK_BASE}/scripts/bin2c.py ${EXECUTABLE_OUTPUT_PATH}/${APP_BIN_NAME} sec_core_img > ${c_array_path}
    )
endfunction ()
