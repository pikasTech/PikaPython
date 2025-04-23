# Copyright (c) 2023-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import re
import sys
import yaml

EXCLUDED_TARGETS="excluded_targets"

def unify_target_name(name):
    unified_name = name
    if re.match(r'^(debug|release)', name):
        unified_name = "ram_" + name
    elif not name.endswith("debug") and not name.endswith("release"):
        unified_name = name + "_debug"
    return unified_name

def get_excluded_targets(input_yml):
    excluded = []
    with open(input_yml, "r", encoding="utf-8", errors="ignore") as stream:
        try:
            info = yaml.safe_load(stream)
            if not info is None and EXCLUDED_TARGETS in info.keys():
                for t in info[EXCLUDED_TARGETS]:
                    excluded.append(unify_target_name(t.strip().lower()))
        except yaml.YAMLError as e:
            pass
    stream.close()
    if len(excluded):
        sys.stdout.write("%s\n" % (";".join(excluded)))

    return excluded

def check_excluded_targets(input_yml, build_type):
    sys.stderr.write(build_type + "\n")
    exc_tar = get_excluded_targets(input_yml)
    if len(exc_tar) != 0:
        build_type = unify_target_name(build_type)
        if build_type in exc_tar:
            sys.exit(1)
        else:
            sys.exit(0)
    else:
        sys.exit(0)

if __name__ == "__main__":
    get_excluded_targets(sys.argv[1])
