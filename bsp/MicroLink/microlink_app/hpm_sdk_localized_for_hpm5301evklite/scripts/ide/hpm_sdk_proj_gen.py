# Copyright (c) 2023-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

#!/usr/bin/env python3

# Usage: python embedded_studio_proj_gen.py <target_configuration>.json <output_directory>
#
# <target_configuration>.json is a json file generated from CMake on the form:
# {
#     "target": {
#         "name": "hello_world",
#         "sources": "hello_world.c;..",
#         "includes": "include1;include2;..",
#         "definitions":"NDEBUG;FLASH_XIP=1;..",
#     },
#     "platform": {
#         "name": "yitian",
#         "arch": "rv32",
#         "flash_size": 524288,
#         "ram_size": 65536,
#     }
# }

import jinja2
import sys
import argparse
import json
import os
import re

class ProjGenHelper:
    hpm_sdk_base = None
    exclude_file_list = [];
    def __init__(self, sdk_base, exclude_file_list = []):
        self.hpm_sdk_base = sdk_base
        self.exclude_file_list = exclude_file_list

    def load_config(self, input_file):
        with open(input_file, "r") as f:
            config = json.load(f)
        return config

    def load_platform(self, platform_config):
        with open(platform_config["definition_file"], "r") as f:
            config = json.load(f)
        return [platform for platform in config["platforms"] if platform["name"] == platform_config["name"]][0]

    def file_to_be_excluded(self, f):
        for e in self.exclude_file_list:
            if e in f:
                return True
        return False

    def fix_path(self, p, expand_path = True):
        if expand_path:
            p = os.path.realpath(os.path.expanduser(p))
        return re.sub(r'\\', r'/', p)

    def get_relpath(self, f, base):
        try:
            return os.path.relpath(os.path.realpath(f), os.path.realpath(base))
        except ValueError:
            return f

    def get_sdk_fullpath(self, f, sdk_base):
        # replace real path with sdk base variable to be defined in SES
        sdk_relpath = self.get_relpath(f, sdk_base)
        if sdk_relpath == f:
            return self.fix_path(f)
        else:
            return "/".join([self.hpm_sdk_base, self.fix_path(sdk_relpath, False)])

    def file_in_directory(self, f, directory):
        tmp_f = re.sub(r'-', r'_HpM_', f)
        tmp_dir = re.sub(r'-', r'_HpM_', directory)
        r = re.sub(re.escape(tmp_dir), r'', tmp_f)
        return not (r == tmp_f)

    def is_sdk_file(self, f, sdk_base):
        return self.file_in_directory(os.path.realpath(f), os.path.realpath(sdk_base))

    def is_sdk_target_file(self, f, sdk_base = "", target = ""):
        tmp = re.sub(re.escape(sdk_base) + r'[\\/]', '', f)
        return re.search(re.escape(target), tmp)

    def is_sdk_sample_file(self, f, sdk_base = ""):
        return self.is_sdk_target_file(f, sdk_base, "samples")

    def is_sdk_unittest_file(self, f, sdk_base = ""):
        return self.is_sdk_target_file(f, sdk_base, "unit_test")

    def get_file_path(self, file, sdk_base, out_dir, use_outdir_relpath = True, relpath_prefix = None):
        if use_outdir_relpath:
            p = self.get_relpath(file, out_dir)
            if relpath_prefix:
                p = os.path.join(relpath_prefix, p)
        else:
            p = self.get_sdk_fullpath(file, sdk_base)
        return p


    def get_include_path(self, config, sdk_base, out_dir, defaults = [], use_outdir_relpath = True, relpath_prefix = None):
        l = defaults
        for d in config["target"]["includes"].split(","):
            d = d.strip()
            if len(d) == 0:
                continue
            d = self.fix_path(d)
            inc_path = self.get_file_path(d, sdk_base, out_dir, use_outdir_relpath)
            inc_path = re.sub(r'\\', '/', inc_path)
            if use_outdir_relpath:
                if relpath_prefix is not None:
                    inc_path = os.path.join(relpath_prefix, inc_path)
            l.append(inc_path)
        return l

    def get_definitions(self, definitions):
        l = []
        for i in definitions.split(","):
            l.append(re.sub("\"", "&quot;", i))
        return l

    def get_link_symbols(self, symbols):
        l = []
        for i in symbols.split(","):
            l.append(re.sub("\"", "&quot;", i))
        return l

    def get_app_common_path(self, file_full_path, project_dir):
        app_common_path = os.path.commonprefix([project_dir, file_full_path])
        if len(app_common_path) == 0:
            app_common_path = project_dir
        # remove tailing "/"
        return re.sub(r'/$', '', app_common_path)

    def is_custom_board_file(self, f, board_dir):
        return self.file_in_directory(f, board_dir)


    def dict_get_key_value_with_default(self, dict, key, default_value):
        if not key in dict.keys() or not len(dict[key].strip()) > 0:
            return default_value
        else:
            return dict[key]
