#!/usr/bin/env python3

# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys
import json

excluded_file_path_list = ['toolchains/gcc/start.S']

def path_conv_to_linux_format(str):
    return str.replace('\\', '/')
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
