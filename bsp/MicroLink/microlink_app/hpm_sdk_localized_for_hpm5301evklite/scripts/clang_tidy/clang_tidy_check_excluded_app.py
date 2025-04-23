# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import sys
import yaml

def check_excluded_app(input_yml, app):
    excluded = False
    with open(input_yml, 'r', encoding='utf-8', errors='ignore') as stream:
        try:
            info = yaml.safe_load(stream)
            if not info is None:
                for t in info['excluded_sample']:
                    if t in app:
                        excluded = True
                        break
        except yaml.YAMLError as e:
            print("Load {} failed!".format(input_yml))
            stream.close()
            sys.exit(2)
    stream.close()

    if excluded:
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == "__main__":
    check_excluded_app(sys.argv[1], sys.argv[2])
