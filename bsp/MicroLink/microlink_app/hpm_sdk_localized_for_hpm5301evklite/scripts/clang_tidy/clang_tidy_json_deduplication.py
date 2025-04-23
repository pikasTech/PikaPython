#!/usr/bin/env python3

# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys
import yaml
import json

excluded_file_path_list = []
included_file_path_list = []

def path_conv_to_linux_format(str):
    return str.replace('\\', '/')

def load_excluded_and_included_info(input_yml):
    with open(input_yml, 'r', encoding='utf-8', errors='ignore') as stream:
        try:
            info = yaml.safe_load(stream)
        except yaml.YAMLError as e:
            print("Load {} failed!".format(input_yml))
            stream.close()
            sys.exit(1)
    stream.close()
    return info

def load_json_info(json_file):
    with open(json_file, 'r', encoding='utf-8', errors='ignore') as stream:
        try:
            info = json.load(stream)
        except json.JSONDecodeError as e:
            print("Load {} failed".format(json_file))
            stream.close()
            sys.exit(1)
    stream.close()
    return info

if __name__ == "__main__":
    if (len(sys.argv) < 2):
        print("sys.argv:", sys.argv)
        print("Error: no yaml file input")
        sys.exit(1)

    # load yaml file
    yaml = load_excluded_and_included_info(sys.argv[1])
    excluded_file_path_list = yaml['excluded_file_path']
    included_file_path_list = yaml['included_file_path']

    # delete wildcards for excluded file path
    if excluded_file_path_list:
        for idx in range(len(excluded_file_path_list)):
            excluded_file_path_list[idx] = path_conv_to_linux_format(excluded_file_path_list[idx])
            if excluded_file_path_list[idx][-1] == '*':
               excluded_file_path_list[idx] = excluded_file_path_list[idx][:-1]
    else:
        print("No excluded file path input!")
        sys.exit(1)

    # delete wildcards for included file path
    if included_file_path_list:
        for idx in range(len(included_file_path_list)):
            included_file_path_list[idx] = path_conv_to_linux_format(included_file_path_list[idx])
            if included_file_path_list[idx][-1] == '*':
               included_file_path_list[idx] = included_file_path_list[idx][:-1]

    # load json file
    list_json = load_json_info('compile_commands.json')
    list_json_excluded_idx = []
    list_json_remaining_file = []

    for idx in range(len(list_json)):

        # check excluded file path to get excluded file index
        for excluded_list_idx in range(len(excluded_file_path_list)):

            # convert path to linux format
            if (os.sep == '\\'):
                list_json[idx]["file"] = path_conv_to_linux_format(list_json[idx]["file"])

            if (excluded_file_path_list[excluded_list_idx] in list_json[idx]["file"]):
                if included_file_path_list:
                    for included_list_idx in range(len(included_file_path_list)):
                        if (included_file_path_list[included_list_idx] in list_json[idx]["file"]):
                            break
                        else:
                            continue
                    else:
                        list_json_excluded_idx.append(idx)
                        break
                else:
                    list_json_excluded_idx.append(idx)
                    break
    else:
        # generate a list of the remaining file
        for idx in range(len(list_json)):
            if idx not in list_json_excluded_idx:
                list_json_remaining_file.append(list_json[idx:idx+1][0])

    # write json file
    with open('data.json', 'w') as f:
        json.dump(list_json_remaining_file, f)
    f.close()

    if os.path.exists('compile_commands_bkp.json'):
        os.remove('compile_commands_bkp.json')

    os.rename('compile_commands.json', 'compile_commands_bkp.json')
    os.rename('data.json', 'compile_commands.json')
    os.listdir()
    sys.exit(0)
