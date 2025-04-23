# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import sys
import yaml

def check_excluded_app(input_yml, board, app):
    excluded = False
    with open(input_yml, 'r', encoding='utf-8', errors='ignore') as stream:
        try:
            info = yaml.safe_load(stream)
            if not info is None and board in info.keys():
                for t in info[board]:
                    if t == app:
                        excluded = True;
                        break
        except yaml.YAMLError as e:
            pass
    stream.close()
    if excluded:
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == "__main__":
    check_excluded_app(sys.argv[1], sys.argv[2], sys.argv[3])
