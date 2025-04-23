# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import sys
import yaml

def check_nds_toolchain(input_yml, toolchain, app):
    nds_gcc = False
    with open(input_yml, 'r', encoding='utf-8', errors='ignore') as stream:
        try:
            info = yaml.safe_load(stream)
            if not info is None and toolchain in info.keys():
                for t in info[toolchain]:
                    if t == app:
                        nds_gcc = True
                        break
        except yaml.YAMLError as e:
            pass
    stream.close()
    if nds_gcc:
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == "__main__":
    check_nds_toolchain(sys.argv[1], sys.argv[2], sys.argv[3])
