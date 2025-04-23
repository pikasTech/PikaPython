# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_options("-Wall")
sdk_compile_options("-Wundef")
sdk_compile_options("-Wno-format")
sdk_compile_options("-fomit-frame-pointer")
sdk_compile_options("-fno-builtin")
sdk_compile_options("-ffunction-sections")
sdk_compile_options("-fdata-sections")

include(${HPM_SDK_BASE}/cmake/toolchain/lld.cmake)

