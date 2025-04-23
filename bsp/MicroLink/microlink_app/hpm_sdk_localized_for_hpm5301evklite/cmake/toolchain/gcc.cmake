# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_options("-Wall")
sdk_compile_options("-Wundef")
sdk_compile_options("-Wno-format")
sdk_compile_options("-fomit-frame-pointer")
sdk_compile_options("-fno-builtin")
sdk_compile_options("-ffunction-sections")
sdk_compile_options("-fdata-sections")

if("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
# By default, Andes' gcc toolchain uses lld as linker
include(${HPM_SDK_BASE}/cmake/toolchain/lld.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "gcc")
include(${HPM_SDK_BASE}/cmake/toolchain/ld.cmake)

if(NOT HPM_SDK_LD_NO_NANO_SPECS)
    sdk_ld_options("--specs=nano.specs")
    sdk_ld_options("-u _printf_float")
    sdk_ld_options("-u _scanf_float")
endif()

endif()

