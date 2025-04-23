# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import sys
import yaml

MIN_SDK_VERSION="minimum_sdk_version"

def get_min_sdk_version(input_yml):
    min_version = ""
    with open(input_yml, "r", encoding="utf-8", errors="ignore") as stream:
        try:
            info = yaml.safe_load(stream)
            if not info is None and MIN_SDK_VERSION in info.keys() and info[MIN_SDK_VERSION] is not None:
                for t in info[MIN_SDK_VERSION]:
                    min_version = (t.strip().lower())
        except yaml.YAMLError as e:
            pass
    stream.close()
    if len(min_version):
        sys.stdout.write("%s\n" % (min_version))

    return min_version

if __name__ == "__main__":
    get_min_sdk_version(sys.argv[1])
