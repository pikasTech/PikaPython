# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import sys
import yaml

DEBUG_INFO="debug"
DEBUG_INFO_SES="ses"
DEBUG_INFO_SES_AUTO_START_GDB_SERVER="auto_start_gdb_server"
DEBUG_INFO_SES_GDB_SERVER_PORT="gdb_server_port"
DEBUG_INFO_SES_GDB_SERVER_RST_CMD="gdb_server_reset_command"

def get_ses_info(input_yml, info_type = "auto_start_gdb_server"):
    gdb_auto_start = "Yes"
    gdb_server_port = 3333
    gdb_server_reset_command = "reset halt"
    with open(input_yml, "r", encoding="utf-8", errors="ignore") as stream:
        try:
            info = yaml.safe_load(stream)
            if not info is None and DEBUG_INFO in info.keys() and DEBUG_INFO_SES in info[DEBUG_INFO].keys():
                if DEBUG_INFO_SES_AUTO_START_GDB_SERVER in info[DEBUG_INFO][DEBUG_INFO_SES].keys():
                    r = info[DEBUG_INFO][DEBUG_INFO_SES][DEBUG_INFO_SES_AUTO_START_GDB_SERVER]
                    if not r:
                        gdb_auto_start = "No"
                if DEBUG_INFO_SES_GDB_SERVER_RST_CMD in info[DEBUG_INFO][DEBUG_INFO_SES].keys():
                    gdb_server_reset_command = info[DEBUG_INFO][DEBUG_INFO_SES][DEBUG_INFO_SES_GDB_SERVER_RST_CMD]
                if DEBUG_INFO_SES_GDB_SERVER_PORT in info[DEBUG_INFO][DEBUG_INFO_SES].keys():
                    gdb_server_port = info[DEBUG_INFO][DEBUG_INFO_SES][DEBUG_INFO_SES_GDB_SERVER_PORT]
        except yaml.YAMLError as e:
            pass
    stream.close()
    if info_type.lower() == "auto_start_gdb_server":
        sys.stdout.write("%s" % (gdb_auto_start))
    elif info_type.lower() == "gdb_server_port":
        sys.stdout.write("%d" % (gdb_server_port))
    elif info_type.lower() == "gdb_server_reset_command":
        sys.stdout.write("%s" % (gdb_server_reset_command))

if __name__ == "__main__":
    if sys.argv[2].lower() == "ses":
        get_ses_info(sys.argv[1], sys.argv[3].lower())
    else:
        sys.stdout.write("unsupported tool: %s" % (sys.argv[2].lower()))
        sys.exit(2)
