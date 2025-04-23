# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import re
import sys

def get_gcc_version(version_text):
    version_text = re.sub(r'\n', r' ', version_text)
    m = re.match(r'.*\s+([0-9]+\.[0-9]+\.[0-9]+)\s*', version_text)
    if m is not None:
        print(m.group(1))
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    get_gcc_version(sys.argv[1])
