#!/usr/bin/env python3

# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys

EXCLUDED_SAMPLE_NAME="hello_world"

sys.path.append(os.path.dirname(sys.argv[0]))

if __name__ == "__main__":
    if (os.environ.get('HPM_SDK_BASE')==None):
        print("Please set HPM_SDK_BASE environment variable")
        sys.exit(1)

    if (os.environ.get('CLANG_TIDY_EXTRA_ARG')==None):
        print("Please set CLANG_TIDY_EXTRA_ARG environment variable")
        sys.exit(1)

    if (EXCLUDED_SAMPLE_NAME not in os.getcwd()):
        os.system('{}/scripts/clang_tidy/clang_tidy_json_deduplication.py {}/scripts/clang_tidy/.clang_tidy_excluded_included_path.yaml'.format(os.environ.get('HPM_SDK_BASE'), os.environ.get('HPM_SDK_BASE')))
    else:
        os.system('{}/scripts/clang_tidy/clang_tidy_json_full_check_sample_handle.py'.format(os.environ.get('HPM_SDK_BASE')))

    ret = os.system('run-clang-tidy.py {} -config-file={}/scripts/clang_tidy/.clang-tidy -warnings-as-errors=*'.format(os.environ.get('CLANG_TIDY_EXTRA_ARG'), os.environ.get('HPM_SDK_BASE')))
    sys.exit(ret)

