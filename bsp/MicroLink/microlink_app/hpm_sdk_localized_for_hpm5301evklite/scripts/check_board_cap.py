# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import re
import sys
import yaml
import subprocess
import shutil

sys.path.append(os.path.dirname(sys.argv[0]))
import get_board_info

APP_DEPENDENCY="dependency"
APP_LINKED_PROJECT="linked_project"
APP_LINKED_PROJECT_NAME="project_name"
APP_LINKED_PROJECT_PATH="project_path"
APP_LINKED_PROJECT_BUILD_TYPE="build_type"
SOC_IP_FEATURE_FILE_NAME="hpm_soc_ip_feature.h"
SOC_IP_FEATURE_PREFIX="HPM_IP_FEATURE_"

def parse_app_yml(app_yml):
    app_info = None
    with open(app_yml, "r", encoding='utf-8', errors='ignore') as stream:
        try:
            app_info = yaml.safe_load(stream)
        except yaml.YAMLError as e:
            pass
    stream.close()
    return app_info

def get_app_dep(app_info):
    app_dep = None
    if not app_info is None and APP_DEPENDENCY in app_info.keys():
        app_dep = app_info[APP_DEPENDENCY]
    return app_dep

def get_linked_project(app_info):
    linked_proj_info = None
    if not app_info is None and APP_LINKED_PROJECT in app_info.keys():
        linked_proj_info = app_info[APP_LINKED_PROJECT]
    return linked_proj_info

def is_sdk_sample(sdk_base, app_path):
    return re.match(r'^' + re.escape(sdk_base) + r'/samples/', re.sub(r'\\', '/', app_path))

def is_sdk_board(sdk_base, board_dir):
    return re.match(r'^' + re.escape(sdk_base) + r'/boards/', re.sub(r'\\', '/', board_dir))

def build_linked_project(sdk_base, app_info, board_name, board_dir, app_yml_base):
    project_name=""
    project_path=""
    build_type=""
    linked_proj_info = get_linked_project(app_info)
    linked_proj_root_dir = ""
    if (linked_proj_info == None):
        return 0
    else:
        if APP_LINKED_PROJECT_NAME in linked_proj_info.keys():
            project_name = linked_proj_info[APP_LINKED_PROJECT_NAME]
        if APP_LINKED_PROJECT_PATH in linked_proj_info.keys():
            project_path = linked_proj_info[APP_LINKED_PROJECT_PATH]
        if APP_LINKED_PROJECT_BUILD_TYPE in linked_proj_info.keys():
            build_type = linked_proj_info[APP_LINKED_PROJECT_BUILD_TYPE]
        if APP_LINKED_PROJECT_PATH in linked_proj_info.keys():
            linked_proj_root_dir = linked_proj_info[APP_LINKED_PROJECT_PATH]
        else:
            linked_proj_root_dir = os.path.join(sdk_base, "samples")

        if project_path != "":
            if not os.path.isabs(project_path):
                p = os.path.realpath(os.path.join(app_yml_base, project_path))
            else:
                p = os.path.realpath(project_path)
            if os.path.exists(p):
                linked_proj_root_dir = p
        if (project_name != "" and build_type != ""):
            if linked_proj_root_dir == "":
                linked_proj_root_dir = os.path.join(sdk_base, "samples", project_name)
            linked_proj_build_dir = os.path.join(linked_proj_root_dir, "build")
            if os.path.exists(linked_proj_build_dir):
                shutil.rmtree(linked_proj_build_dir)
            os.mkdir(linked_proj_build_dir)
            os.chdir(linked_proj_build_dir)
            extra_option = ""
            if not is_sdk_board(sdk_base, board_dir):
                extra_option = "-DBOARD_SEARCH_PATH=" + os.path.dirname(board_dir)
            print('-- Started to build core1 project...')
            build_linked_proj_cmd = "cmake -GNinja -DBOARD=" + board_name + " -DHPM_BUILD_TYPE=" + build_type + " " + extra_option + " -B " + linked_proj_build_dir + " -S " + linked_proj_root_dir
            build_linked_proj_cmd += " && ninja"
            p = subprocess.Popen(build_linked_proj_cmd, shell=True,  stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            retval = p.wait()
            if retval != 0:
                print("-- Failed to build core1 project!")
            else:
                print('-- Finished building core1 project successfully!')
            return retval

def get_soc_ip_feature_list(sdk_base, soc_series, soc_name):
    soc_ip_feature_file = os.path.join(sdk_base, "soc", soc_series, soc_name, SOC_IP_FEATURE_FILE_NAME)
    feature_list = []
    if os.path.exists(soc_ip_feature_file):
        with open(soc_ip_feature_file, 'r', encoding='utf-8', errors='ignore') as stream:
            for line in stream.readlines():
                regex_result = re.match(r'^#define\s+(%s.*)\s+1' % SOC_IP_FEATURE_PREFIX, line)
                if regex_result:
                    feature_list.append(regex_result.group(1))
        stream.close()
    return feature_list


def check_ip_dependency(sdk_base, soc_name, app_dependency):
    soc_series = get_board_info.get_soc_series_name(soc_name)
    # check ip dependency
    ip_list = get_board_info.get_soc_ip_list(soc_series, soc_name)

    feature_list = get_soc_ip_feature_list(sdk_base, soc_series, soc_name)
    for d in app_dep:
        if not re.match('ip_feature_', d):
            # process OR logic
            m = re.match(r'||', d)
            if not m is None:
                or_list = d.split('||')
                found = False
                for d in or_list:
                    if d.strip() in board_cap.split(get_board_info.BOARD_FEATURE_DELIM):
                        found = True
                        break
                    if len(ip_list) > 0:
                        if d.strip() in ip_list:
                            found = True
                            break
                if not found:
                    return False
            else:
                if not d.strip() in board_cap.split(get_board_info.BOARD_FEATURE_DELIM) and not d.strip() in ip_list:
                    return False
        else:
            if not ("HPM_%s" % d).upper() in feature_list:
                return False
    return True


if __name__ == "__main__":
    board_cap = get_board_info.get_info(sys.argv[1], get_board_info.BOARD_INFO_FEATURE_KEY)
    board_name = os.path.splitext(os.path.basename(sys.argv[1]))[0]
    board_excluded_samples = get_board_info.get_info(sys.argv[1], get_board_info.BOARD_INFO_EXCLUDED_SAMPLES_KEY)
    soc_name = get_board_info.get_info(sys.argv[1], get_board_info.BOARD_INFO_SOC_KEY)
    board_dir = os.path.dirname(sys.argv[1])
    app_yml = os.path.realpath(sys.argv[2])

    if not os.path.exists(app_yml) or board_cap is None:
        sys.exit(0)

    sdk_base = os.path.realpath(os.environ["HPM_SDK_BASE"])
    app_path = os.path.realpath(app_yml)

    if is_sdk_sample(sdk_base, os.path.dirname(app_path)):
        try:
            app_name = os.path.dirname(os.path.relpath(app_path, sdk_base))
            if app_name in board_excluded_samples:
                sys.exit(1)
        except:
            # if application is not on the same drive with sdk_base, skip checking
            pass

    app_info = parse_app_yml(app_yml)

    # Check app dependency and IP dependency first
    app_dep = get_app_dep(app_info)
    if app_dep is None:
        sys.exit(0)

    if not check_ip_dependency(sdk_base, soc_name, app_dep):
        sys.exit(1)

    # Build linked project if all dependencies are met
    retval = build_linked_project(sdk_base, app_info, board_name, board_dir, os.path.dirname(app_yml))
    if (retval != 0):
        sys.exit(2)


    sys.exit(0)
