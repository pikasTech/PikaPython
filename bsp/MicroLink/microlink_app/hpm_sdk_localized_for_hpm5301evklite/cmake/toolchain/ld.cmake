# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_ld_options("-static")
sdk_ld_options("-nostartfiles")
sdk_ld_options("-Wl,--gc-sections")
sdk_ld_options("-Wl,--print-memory-usage")
