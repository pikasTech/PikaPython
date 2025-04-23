# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import re
import sys
import yaml

BOARD_INFO_SOC_KEY="soc"
BOARD_INFO_DEVICE_NAME_KEY="device"
BOARD_INFO_OPENOCD_SOC_KEY="openocd-soc"
BOARD_INFO_OPENOCD_PROBE_KEY="openocd-probe"
BOARD_INFO_FEATURE_KEY="feature"
BOARD_INFO_FLASH_KEY="on-board-flash"
BOARD_INFO_EXTRAM_KEY="on-board-ram"
BOARD_INFO_SIZE_KEY="size"
BOARD_INFO_EXCLUDED_SAMPLES_KEY="excluded_samples"
BOARD_FEATURE_DELIM=":"
SOC_MODULES_LIST_NAME = 'soc_modules.list'

# get soc series name according to file origination
def get_soc_series_name(soc_name):
    soc_series_name = ""
    soc_dir = os.path.join(os.environ['HPM_SDK_BASE'], "soc")
    subdirectories = [d for d in os.listdir(soc_dir) if os.path.isdir(os.path.join(soc_dir, d))]

    for subdir in subdirectories:
        subdir_path = os.path.join(soc_dir, subdir)
        if os.path.isdir(subdir_path):
            sub_subdirectories = [d for d in os.listdir(subdir_path) if os.path.isdir(os.path.join(subdir_path, d))]
            if soc_name in sub_subdirectories:
                soc_series_name = subdir
                break
    return soc_series_name


def get_soc_ip_list(soc_series, soc_name):
    ip_list = []
    module_list = os.path.join(os.environ['HPM_SDK_BASE'], "soc", soc_series, soc_name, SOC_MODULES_LIST_NAME)
    with open(module_list, "r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()
        for l in lines:
            if ip := re.match(r'HPMSOC_HAS_HPMSDK_(\w+)', l):
                ip_list.append(ip.group(1).lower())
    f.close()
    return ip_list

def get_info(board_yml, info_type = "soc"):
    result = "not found"
    board_info = None
    with open(board_yml, "r", encoding="utf-8", errors="ignore") as stream:
        try:
            board_info = yaml.safe_load(stream)
            if(info_type == "soc"):
                if BOARD_INFO_SOC_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_SOC_KEY])
            elif(info_type == "openocd-soc"):
                if BOARD_INFO_OPENOCD_SOC_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_OPENOCD_SOC_KEY])
            elif(info_type == "openocd-probe"):
                if BOARD_INFO_OPENOCD_PROBE_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_OPENOCD_PROBE_KEY])
            elif(info_type == "feature"):
                if BOARD_INFO_FEATURE_KEY in board_info["board"].keys():
                    result = BOARD_FEATURE_DELIM.join(board_info["board"][BOARD_INFO_FEATURE_KEY])
            elif(info_type == "flash_size"):
                if BOARD_INFO_FLASH_KEY in board_info["board"].keys():
                    result = board_info["board"][BOARD_INFO_FLASH_KEY][BOARD_INFO_SIZE_KEY]
            elif(info_type == "extram_size"):
                if BOARD_INFO_EXTRAM_KEY in board_info["board"].keys():
                    result = board_info["board"][BOARD_INFO_EXTRAM_KEY][BOARD_INFO_SIZE_KEY]
            elif(info_type == "device"):
                if BOARD_INFO_DEVICE_NAME_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_DEVICE_NAME_KEY])
            elif(info_type == "excluded_samples"):
                if BOARD_INFO_EXCLUDED_SAMPLES_KEY in board_info["board"].keys():
                    result = BOARD_FEATURE_DELIM.join(board_info["board"][BOARD_INFO_EXCLUDED_SAMPLES_KEY])
        except yaml.YAMLError as e:
            pass
    stream.close()
    return result

if __name__ == "__main__":
    sys.stdout.write(get_info(sys.argv[1], sys.argv[2].lower()))
