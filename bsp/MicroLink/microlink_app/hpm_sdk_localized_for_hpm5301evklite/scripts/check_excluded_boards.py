# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import sys
import yaml

EXCLUDED_boards="excluded_boards"

def get_excluded_boards(input_yml):
    excluded = []
    with open(input_yml, 'r', encoding='utf-8', errors='ignore') as stream:
        try:
            info = yaml.safe_load(stream)
            if not info is None and EXCLUDED_boards in info.keys():
                for t in info[EXCLUDED_boards]:
                    excluded.append(t.strip().lower())
        except yaml.YAMLError as e:
            pass
    stream.close()
    if len(excluded):
        sys.stdout.write("%s\n" % (";".join(excluded)))

    return excluded

def check_excluded_boards(input_yml, board_type):
    exc_tar = get_excluded_boards(input_yml)
    if len(exc_tar) != 0:
        if board_type in exc_tar:
            sys.exit(1)
        else:
            sys.exit(0)
    else:
        sys.exit(0)

if __name__ == "__main__":
    get_excluded_boards(sys.argv[1])