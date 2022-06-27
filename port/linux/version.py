import sys
import os
import time
from string import Template
import traceback
import version_config

VersionText = """#define PIKA_VERSION_MAJOR       ${MajorVersion}
#define PIKA_VERSION_MINOR       ${MinorVersion}
#define PIKA_VERSION_MICRO       ${MicroVersion}

#define PIKA_EDIT_TIME      ${BuildTime}
"""

SVNReposPath = os.getcwd()

try:
    MajorVersion = version_config.MajorVersion
    MinorVersion = version_config.MinorVersion
    MicroVersion = version_config.MicroVersion

    BuildTime = '\"' + \
        time.strftime("%Y/%m/%d %H:%M:%S", time.localtime()) + '\"'

    try:
        os.remove("PikaVersion.h")
    except:
        pass

    VersionValue = {"MajorVersion": MajorVersion,
                    "MinorVersion": MinorVersion,  "MicroVersion": MicroVersion}
    VersionValue.update(
        {'BuildTime': BuildTime})

    t = Template(VersionText)

    fwrite = open("PikaVersion.h",  "w")
    fwrite.write(t.safe_substitute(VersionValue))
    fwrite.close()
    print("make version file success")
except:
    traceback.print_exc(file=sys.stdout)
    print("make version file failed")
