# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys
import yaml

APP_INFO_LINKED_PROJECT_NAME = "linked_project_name"
APP_INFO_LINKED_PROJECT_PATH = "linked_project_path"

MIN_SDK_VERSION="minimum_sdk_version"
LINKED_PROJECT="linked_project"
LINKED_PROJECT_PATH="project_path"
LINKED_PROJECT_NAME="project_name"
LINKED_PROJECT_BUILD_TYPE="build_type"

EXCLUDED_IDES="excluded_ides"

DEBUG_INFO="debug"
DEBUG_INFO_SES="ses"
DEBUG_INFO_SES_AUTO_START_GDB_SERVER="auto_start_gdb_server"
DEBUG_INFO_SES_GDB_SERVER_PORT="gdb_server_port"
DEBUG_INFO_SES_GDB_SERVER_RST_CMD="gdb_server_reset_command"

def parse_app_yml(app_yml):
    app_info = None
    with open(app_yml, "r", encoding="utf-8", errors="ignore") as stream:
        try:
            app_info = yaml.safe_load(stream)
        except yaml.YAMLError as e:
            pass
    stream.close()
    return app_info

def get_min_sdk_version(app_info):
    min_version = ""
    if not app_info is None and MIN_SDK_VERSION in app_info.keys() and app_info[MIN_SDK_VERSION] is not None:
        for t in app_info[MIN_SDK_VERSION]:
            min_version = (t.strip().lower())
    if len(min_version):
        sys.stdout.write("%s\n" % (min_version))

    return min_version

def get_linked_project(app_info):
    linked_proj_info = None
    if not app_info is None and LINKED_PROJECT in app_info.keys():
        linked_proj_info = app_info[LINKED_PROJECT]
    return linked_proj_info

def get_excluded_ides(app_info):
    excluded_ides = []
    if not app_info is None and EXCLUDED_IDES in app_info.keys():
        excluded_ides = app_info[EXCLUDED_IDES]
    return excluded_ides

def get_ses_info(app_info, info_type = "auto_start_gdb_server"):
    gdb_auto_start = "Yes"
    gdb_server_port = 3333
    gdb_server_reset_command = "reset halt"

    if not app_info is None and DEBUG_INFO in app_info.keys() and DEBUG_INFO_SES in app_info[DEBUG_INFO].keys():
        if DEBUG_INFO_SES_AUTO_START_GDB_SERVER in app_info[DEBUG_INFO][DEBUG_INFO_SES].keys():
            r = app_info[DEBUG_INFO][DEBUG_INFO_SES][DEBUG_INFO_SES_AUTO_START_GDB_SERVER]
            if not r:
                gdb_auto_start = "No"
        if DEBUG_INFO_SES_GDB_SERVER_RST_CMD in app_info[DEBUG_INFO][DEBUG_INFO_SES].keys():
            gdb_server_reset_command = app_info[DEBUG_INFO][DEBUG_INFO_SES][DEBUG_INFO_SES_GDB_SERVER_RST_CMD]
        if DEBUG_INFO_SES_GDB_SERVER_PORT in app_info[DEBUG_INFO][DEBUG_INFO_SES].keys():
            gdb_server_port = app_info[DEBUG_INFO][DEBUG_INFO_SES][DEBUG_INFO_SES_GDB_SERVER_PORT]
    gdb_debug_info = "%s:%s,%s:%s,%s"

    if info_type.lower() == "auto_start_gdb_server":
        sys.stdout.write("%s" % (gdb_auto_start))
    elif info_type.lower() == "gdb_server_port":
        sys.stdout.write("%d" % (gdb_server_port))
    elif info_type.lower() == "gdb_server_reset_command":
        sys.stdout.write("%s" % (gdb_server_reset_command))

def get_app_info(app_yml, info_type, write_to_stdout = False):
    value = ""
    app_info = parse_app_yml(app_yml)
    if app_info is None:
        return False, value
    if info_type in [APP_INFO_LINKED_PROJECT_NAME, APP_INFO_LINKED_PROJECT_PATH]:
        linked_proj_info = get_linked_project(app_info)
        if linked_proj_info is None:
            return False, value
        if info_type == APP_INFO_LINKED_PROJECT_NAME:
            if LINKED_PROJECT_NAME in linked_proj_info.keys():
                value = linked_proj_info[LINKED_PROJECT_NAME]
                if write_to_stdout:
                    sys.stdout.write("%s" % value)
                return True, value
            else:
                return False, value
        elif info_type == APP_INFO_LINKED_PROJECT_PATH:
            if LINKED_PROJECT_PATH in linked_proj_info.keys():
                value = linked_proj_info[LINKED_PROJECT_PATH]
            else:
                value = os.path.join(os.getenv('HPM_SDK_BASE'), "samples")
            if write_to_stdout:
                sys.stdout.write("%s" % value)
            return True, value
    elif info_type == "excluded_ides":
        excluded_ides = get_excluded_ides(app_info)
        value = ""
        if (not excluded_ides is None) and len(excluded_ides) > 0:
            value = (";".join(excluded_ides))
            if write_to_stdout:
                sys.stdout.write("%s" % value)
        # return True, even if no excluded IDE is found
        return True, value
    return False, value

if __name__ == "__main__":
    if get_app_info(sys.argv[1], sys.argv[2].lower(), True):
        sys.exit(0)
    else:
        sys.exit(1)
