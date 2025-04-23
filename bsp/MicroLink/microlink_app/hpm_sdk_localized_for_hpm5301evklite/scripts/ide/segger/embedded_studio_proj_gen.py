# Copyright (c) 2021-2022,2024 HPMicro
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
from collections import namedtuple
from shutil import copyfile

sys.path.insert(0, os.path.normpath(os.path.join(os.environ.get("HPM_SDK_BASE"), "scripts", "ide")))
from hpm_sdk_proj_gen import ProjGenHelper

HPM_SDK_BASE = "$(HPM_SDK_BASE)"
PROJECT_TEMPLATE = "embedded_studio_proj_tmpl.xml"
HELPER = ProjGenHelper(HPM_SDK_BASE, exclude_file_list = ["sbrk"])

def tree_insert_dirs(ses_dir, ses_file, f, tree):
    dirs = ses_dir.split('/')
    if len(dirs) == 1:
        d = dirs[0]
        if not d in tree.keys():
            tree[d] = {}
        if not "files" in tree[d].keys():
            tree[d]["files"] = []
        tree[d]["files"].append(f)
        return
    d = dirs[0]
    if not d in tree.keys():
        tree[d] = {}
    ses_dir = re.sub(re.escape(d + r'/'), '', ses_dir)
    tree_insert_dirs(ses_dir, ses_file, f, tree[d])
def update_file_tree(sdk_base, out_dir, ses_file, f, tree, use_outdir_relpath):
    ses_dir, file = os.path.split(ses_file)
    if not use_outdir_relpath:
        ses_dir = re.sub(re.escape("%s" % HPM_SDK_BASE) + r'/', '', ses_dir)
    else:
        sdk_base_relpath = re.sub(r'\\', r'/', HELPER.get_relpath(sdk_base, out_dir))
        ses_dir = re.sub(re.escape(sdk_base_relpath) + r'[\\/]?', '', ses_dir)
    tree_insert_dirs(ses_dir, ses_file, f, tree)

def get_separate_intermediate_dir_name(name):
    return "Output/$(Configuration)/Obj/$(ProjectName)/%s" % name

def populate_file_nodes(root, sdk_base, project_dir, custom_board_dir, out_dir, level = 1, separate_intermediate_dir = True, current_folder = None, use_outdir_relpath = False):
    node = ""
    for n in root.keys():
        if n == "files":
            for f in root[n]:
                if HELPER.is_sdk_file(f, sdk_base):
                    fpath = HELPER.get_file_path(f, sdk_base, out_dir, use_outdir_relpath)
                    node += "%s<file file_name=\"%s\">\n" % (" " * (level * 2), re.sub(r'\\', r'/', fpath))
                    obj = re.sub(re.escape(sdk_base) + r'[\\/]?', r'', f)
                elif custom_board_dir is not None and HELPER.is_custom_board_file(f, custom_board_dir):
                    node += "%s<file file_name=\"%s\">\n" % (" " * (level * 2), HELPER.get_relpath(f, out_dir))
                    f = re.sub(r'^\w:', '', f) # remove windows drive letter
                    t = f.split(custom_board_dir)[-1] # get any characters after custom board name
                    t = re.sub(r'^\W', '', t) # remove leading path separator
                    obj = os.path.join(custom_board_dir, t)
                else:
                    node += "%s<file file_name=\"%s\">\n" % (" " * (level * 2), HELPER.get_relpath(f, out_dir))
                    obj = re.sub(re.escape(project_dir), r'app', f)
                    if obj == f:
                        # deal with drive letter of Windows path
                        obj = re.sub(r'^\w:', '', f)
                obj = re.sub(r'\\', r'/', obj)
                if separate_intermediate_dir:
                    obj = os.path.basename(obj)
                node += "%s<configuration Name=\"Common\" build_object_file_name=\"%s$(OBJ)\"/>\n" % (" " * ((level + 1) * 2), "/".join(["$(IntDir)", obj]))
                node += "%s</file>\n" % (" " * (level * 2))
        else:
            node += "%s<folder Name=\"%s\">\n" % (" " * (level * 2), n)
            if separate_intermediate_dir:
                if level == 1 and current_folder is None:
                    tmp_dir = n
                else:
                    tmp_dir = "%s/%s" % (current_folder, n)
                if "files" in root[n].keys():
                    # as long as "files" exists in current node, this folder needs to be taken care of
                    node += "%s<configuration Name=\"Common\" build_intermediate_directory=\"%s\"/>\n" % (" " * ((level + 1) * 2), get_separate_intermediate_dir_name(tmp_dir))
            if separate_intermediate_dir:
                node += populate_file_nodes(root[n], sdk_base, project_dir, custom_board_dir, out_dir, level + 1, current_folder = tmp_dir, use_outdir_relpath = use_outdir_relpath)
            else:
                node += populate_file_nodes(root[n], sdk_base, project_dir, custom_board_dir, out_dir, level + 1, current_folder = current_folder, use_outdir_relpath = use_outdir_relpath)
            node += "%s</folder>\n" % (" " * (level * 2))
    return node

def generate_file_structure(files, sdk_base, out_dir, project_dir, board_dir, board_name, use_outdir_relpath = True):
    f_tree = {}
    app_common_dir = None
    custom_board_dir = None
    # process all files
    for f in files:
        f = f.strip()
        if len(f) == 0:
            continue
        if HELPER.file_to_be_excluded(f):
            continue
        f = HELPER.fix_path(f)
        if HELPER.is_sdk_file(f, sdk_base):
            # sdk source
            if HELPER.is_sdk_sample_file(f, sdk_base):
                ses_file = re.sub(r'\\', r'/', HELPER.get_file_path(f, sdk_base, out_dir, False))
                t = os.path.join(HPM_SDK_BASE, "samples")
                t = re.sub(r'\\', r'/', t)  # convert windows path separator
                ses_file = re.sub(re.escape(t), 'sdk_sample', ses_file)
            elif HELPER.is_sdk_unittest_file(f, sdk_base):
                ses_file = re.sub(r'\\', r'/', HELPER.get_file_path(f, sdk_base, out_dir, False))
                t = os.path.join(HPM_SDK_BASE, "unit_test")
                t = re.sub(r'\\', r'/', t)  # convert windows path separator
                ses_file = re.sub(re.escape(t), 'sdk_unittest', ses_file)
            else:
                ses_file = re.sub(r'\\', r'/', HELPER.get_file_path(f, sdk_base, out_dir, use_outdir_relpath))
        elif HELPER.is_custom_board_file(f, board_dir):
            if custom_board_dir is None:
                custom_board_dir = board_name
            ses_file = re.sub(board_dir, "boards/" + board_name, f)
        else:
            # app source
            if app_common_dir is None:
                d = HELPER.get_app_common_path(f, project_dir)
                if d != project_dir:
                    app_common_dir = d
                else:
                    app_common_dir = project_dir
            m = re.sub(re.escape(app_common_dir), '', f)
            # deal with drive letter of Windows path
            m = re.sub(r'^\w:', '', m)
            ses_file = re.sub(r'^', r'application', m)
        update_file_tree(sdk_base, out_dir, ses_file, f, f_tree, use_outdir_relpath)

    # generate ses project xml content
    nodes = populate_file_nodes(f_tree, sdk_base, app_common_dir, custom_board_dir, out_dir, level = 1, use_outdir_relpath = use_outdir_relpath)
    return nodes

def get_gcc_opt_level(gcc_level):
    if gcc_level == "-O2":
        opt_level_string = "Level 2 balanced"
    elif gcc_level == "-O3":
        opt_level_string = "Level 3 for more speed"
    elif gcc_level == "-O1":
        opt_level_string = "Level 1"
    elif gcc_level == "-O0":
        opt_level_string = "Level 0"
    else:
        opt_level_string = "None"
    return opt_level_string

def get_segger_rtl_linker_symbols(printf, scanf):
    symbols = ""
    prefix = ""
    print_float = False
    scan_float = False

    if printf["fp"]["value"] in ("Float", "Double"):
        if printf["fp"]["value"] == "Float":
            prefix = "__SEGGER_RTL_vfprintf=__SEGGER_RTL_vfprintf_short_float"
        elif printf["fp"]["value"] == "Double":
            prefix = "__SEGGER_RTL_vfprintf=__SEGGER_RTL_vfprintf_float"
        print_float = True
    else:
        prefix = "__SEGGER_RTL_vfprintf=__SEGGER_RTL_vfprintf"

    printf["fmt_level"]["value"] = re.sub(r" ", r"_", printf["fmt_level"]["value"].strip())
    if printf["fmt_level"]["value"] in ("long", "long_long"):
        prefix = "%s_%s" % (prefix, printf["fmt_level"]["value"])
    elif print_float:
        prefix = "%s_long" % (prefix)
    elif not print_float:
        prefix = "%s_int" % (prefix)

    if printf["width_precision"]["value"] in ("No") and not print_float:
        prefix = "%s_nwp" % (prefix)

    if printf["wchar"]["value"] in ("Yes"):
        prefix = "%s_wchar" % (prefix)

    symbols += "%s;" % (prefix)

    if scanf["fp"]["value"] in ("Yes"):
        prefix = "__SEGGER_RTL_vfscanf=__SEGGER_RTL_vfscanf_float"
        scan_float = True
    else:
        prefix = "__SEGGER_RTL_vfscanf=__SEGGER_RTL_vfscanf"

    scanf["fmt_level"]["value"] = re.sub(r" ", r"_", scanf["fmt_level"]["value"].strip())
    if scanf["fmt_level"]["value"] in ("long", "long_long"):
        prefix = "%s_%s" % (prefix, scanf["fmt_level"]["value"])
    elif scan_float:
        prefix = "%s_long" % (prefix)
    elif not scan_float:
        prefix = "%s_int" % (prefix)

    if scanf["cc"]["value"] in ("Yes"):
        symbols += "%s_cc;" % (prefix)
    else:
        symbols += "%s;" % (prefix)

    return symbols

def init_printf_scanf_properties():
    printf = {}
    scanf = {}
    printf["fp"] = {}
    printf["fp"]["name"] = "linker_printf_fp_enabled"
    printf["fp"]["value"] = "Float"
    printf["fmt_level"] = {}
    printf["fmt_level"]["name"] = "linker_printf_fmt_level"
    printf["fmt_level"]["value"] = "int"
    printf["wchar"] = {}
    printf["wchar"]["name"] = "linker_printf_wchar_enabled"
    printf["wchar"]["value"] = "No"
    printf["width_precision"] = {}
    printf["width_precision"]["name"] = "linker_printf_width_precision_supported"
    printf["width_precision"]["value"] = "Yes"

    scanf["fp"] = {}
    scanf["fp"]["name"] = "linker_scanf_fp_enabled"
    scanf["fp"]["value"] = "No"
    scanf["fmt_level"] = {}
    scanf["fmt_level"]["name"] = "linker_scanf_fmt_level"
    scanf["fmt_level"]["value"] = "int"
    scanf["cc"] = {}
    scanf["cc"]["name"] = "linker_scanf_character_group_matching_enabled"
    scanf["cc"]["value"] = "No"
    return printf, scanf

def update_printf_scanf_properties(printf, scanf, name, value):
    if name == printf["fp"]["name"]:
        printf["fp"]["value"] = value
    elif name == printf["fmt_level"]["name"]:
        printf["fmt_level"]["value"] = value
    elif name == printf["wchar"]["name"]:
        printf["wchar"]["value"] = value
    elif name == printf["width_precision"]["name"]:
        printf["width_precision"]["value"] = value
    elif name == scanf["fp"]["name"]:
        scanf["fp"]["value"] = value
    elif name == scanf["fmt_level"]["name"]:
        scanf["fmt_level"]["value"] = value
    elif name == scanf["cc"]["name"]:
        scanf["cc"]["value"] = value

def get_all_printf_scanf_opt_names(printf, scanf):
    opt_names = []
    for i in printf.keys():
        opt_names.append(printf[i]["name"])
    for i in scanf.keys():
        opt_names.append(scanf[i]["name"])
    return opt_names

def popluate_printf_scanf_opts(printf, scanf, opts):
    for i in printf.keys():
        formatted = "%s=\"%s\"" % (printf[i]["name"], printf[i]["value"])
        opts.append(formatted)
    for i in scanf.keys():
        formatted = "%s=\"%s\"" % (scanf[i]["name"], scanf[i]["value"])
        opts.append(formatted)

def process_extra_options(config):
    opts = []
    printf, scanf = init_printf_scanf_properties()
    printf_scanf_opt_names = get_all_printf_scanf_opt_names(printf, scanf)
    for o in config["target"]["extra_ses_options"].split(","):
        o = re.sub(r"^[\"']*(.*)[\"']*$", r'\1', o.strip())
        if not len(o):
            continue
        opt_name = re.sub(r"(.*)=.*", r'\1', o).strip()
        opt_val = re.sub(r".*=\"*(.*)\"*", r'\1', o).strip()
        if opt_name in printf_scanf_opt_names:
            # printf/scanf related properties will be popluated later with default values
            update_printf_scanf_properties(printf, scanf, opt_name, opt_val)
        else:
            formatted = "%s=\"%s\"" % (opt_name, opt_val)
            opts.append(formatted)

    popluate_printf_scanf_opts(printf, scanf, opts)

    config["target"]["extra_ses_options"] = opts
    m = re.match(r'segger', config["target"]["linker_variant"], re.IGNORECASE)
    if m is None:
        # Generate Segger RTL linker symbols, only when linker is not SEGGER
        config["target"]["segger_rtl_linker_symbols"] = get_segger_rtl_linker_symbols(printf, scanf)

def get_openocd_cmdline(config, sdk_base, out_dir, use_outdir_relpath = True):
    if not os.path.exists(config["target"]["openocd"]):
        print("-- Segger openocd is not found")
        return None
    if not "debug_probe" in config["target"].keys():
        print("-- Segger openocd debug probe config is specified")
        return None
    if not "openocd_soc" in config["target"].keys():
        print("-- Segger openocd soc config is specified")
        return None

    openocd_cmdline = HELPER.get_file_path(config["target"]["openocd"], sdk_base, out_dir, use_outdir_relpath)

    probe_cfg = HELPER.get_file_path(os.path.join(sdk_base, "boards", "openocd", "probes", "%s.cfg" % config["target"]["debug_probe"]), sdk_base, out_dir, use_outdir_relpath)
    soc_cfg = HELPER.get_file_path(os.path.join(sdk_base, "boards", "openocd", "soc", "%s.cfg" % config["target"]["openocd_soc"]), sdk_base, out_dir, use_outdir_relpath)

    if use_outdir_relpath:
        probe_cfg = '$(ProjectDir)/' + probe_cfg
        soc_cfg = '$(ProjectDir)/' + soc_cfg

    board_cfg = ""
    if "board_dir" in config["target"].keys():
        if not HELPER.is_sdk_file(config["target"]["board_dir"], sdk_base):
            # board openocd config file should be found at customer board folder named with the same board name
            board_cfg = os.path.join(config["target"]["board_dir"], "%s.cfg" % config["target"]["board"])
            if not os.path.exists(board_cfg):
                board_cfg = ""
        else:
            # locate board openocd config file in sdk
            board_cfg = os.path.join(sdk_base, "boards", "openocd", "boards", "%s.cfg" % config["target"]["board"])
            if os.path.exists(board_cfg):
                board_cfg = HELPER.get_file_path(board_cfg, sdk_base, out_dir, use_outdir_relpath)
                if use_outdir_relpath:
                    board_cfg = '$(ProjectDir)/' + board_cfg
            else:
                board_cfg = ""

    openocd_cmdline = "%s -f %s -f %s" % (config["target"]["openocd"], probe_cfg, soc_cfg)
    openocd_cmdline = re.sub(r'\\', '/', openocd_cmdline)
    if len(board_cfg):
        board_cfg = re.sub(r'\\', '/', board_cfg)
        print("-- Segger openocd board config: %s" % board_cfg)
        return openocd_cmdline + " -f " + board_cfg
    else:
        print("-- Segger openocd board config is not found")
        return openocd_cmdline


def generate_ses_project(config, out_dir=".", project_dir = None):
    files = config["target"]["sources"].split(",")
    sdk_base = HELPER.fix_path(config["target"]["sdk_base"])
    out_dir = HELPER.fix_path(out_dir)

    # use relative path which relatives to out_dir
    use_outdir_relpath = True
    try:
        _ = os.path.relpath(sdk_base, out_dir)
    except ValueError:
        use_outdir_relpath = False

    project_dir = HELPER.fix_path(project_dir)
    board_dir = HELPER.fix_path(config["target"]["board_dir"])
    board_name = config["target"]["board"]

    config["target"]["use_outdir_relpath"] = use_outdir_relpath
    config["target"]["includes"] = HELPER.get_include_path(config, sdk_base, out_dir, [], use_outdir_relpath)
    config["target"]["defines"] = HELPER.get_definitions(config["target"]["defines"])
    config["target"]["file_structure"] = generate_file_structure(files, sdk_base, out_dir, project_dir, board_dir, board_name, use_outdir_relpath)

    config["target"]["link_symbols"] = HELPER.get_link_symbols(config["target"]["link_symbols"])
    config["target"]["linker"] = HELPER.get_file_path(config["target"]["linker"], sdk_base, out_dir, use_outdir_relpath)

    config["target"]["register_definition"] = HELPER.get_file_path(config["target"]["register_definition"], sdk_base, out_dir, use_outdir_relpath)
    config["target"]["cpu_register_definition"] = HELPER.get_file_path(config["target"]["cpu_register_definition"], sdk_base, out_dir, use_outdir_relpath)
    config["target"]["gcc_opt_level"] = get_gcc_opt_level(config["target"]["gcc_opt_level"].strip())

    config["target"]["toolchain_variant"] = config["target"]["toolchain_variant"].title()

    if config["target"]["ses_link_input"].strip():
        config["target"]["ses_link_input"] = HELPER.get_file_path(config["target"]["ses_link_input"], sdk_base, out_dir, use_outdir_relpath)

    if not "compiler_variant" in config["target"].keys() or (len(config["target"]["compiler_variant"].strip()) == 0):
        config["target"]["compiler_variant"] = "SEGGER"

    if not "assembler_variant" in config["target"].keys() or (len(config["target"]["assembler_variant"].strip()) == 0):
        config["target"]["assembler_variant"] = "SEGGER"

    if "linker_variant" in config["target"].keys() and (len(config["target"]["linker_variant"].strip())):
        config["target"]["linker_variant"] = config["target"]["linker_variant"].upper()

    if "openocd" in config["target"].keys():
        config["target"]["openocd_cmdline"] = get_openocd_cmdline(config, sdk_base, out_dir, use_outdir_relpath)
        if config["target"]["openocd_cmdline"] is None:
            config["target"]["openocd"] = ""

    if "post_build_command" in config["target"].keys() and use_outdir_relpath:
        sdk_base_relpath = HELPER.get_file_path(sdk_base, sdk_base, out_dir, use_outdir_relpath)
        sdk_base_relpath = re.sub(r'\\', r'/', sdk_base_relpath)
        cmd = re.sub(r'\$\(HPM_SDK_BASE\)', sdk_base_relpath, config["target"]["post_build_command"])
        config["target"]["post_build_command"] = re.sub(r'\\', r'/', cmd)

    process_extra_options(config)
    s = ""
    with open(os.path.join(sdk_base, "scripts", "ide", "segger", PROJECT_TEMPLATE), "r") as f:
        s = f.read()

    t = jinja2.Template(s)
    s = t.render(config)

    return s

def generate_ses_session(out_dir):
    session_file_contents = ['<!DOCTYPE CrossStudio_Session_File>',
                             '<session>',
                             '\t<Files>',
                             '\t\t<SessionOpenFile path="{}"/>',
                             '\t</Files>',
                             '</session>']
    return '\n'.join(session_file_contents)

def main():
    input_file = sys.argv[1]
    out_dir = sys.argv[2]
    project_dir = sys.argv[3]
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    config = HELPER.load_config(input_file)
    ses_project = generate_ses_project(config, out_dir, project_dir)
    if ses_project is None:
        print("!! Segger: Failed to generate project file")
        sys.exit(1)

    out_dir += "/"
    output_filename = out_dir + config["target"]["name"].replace(".", "_")
    project_file = output_filename + ".emProject"
    with open(project_file, "w") as f:
        f.write(ses_project)

    ses_session = generate_ses_session(out_dir)
    session_file = output_filename + ".emSession"
    with open(session_file, "w") as f:
        f.write(ses_session)

    print("-- Segger Embedded Studio Project: " + project_file)

if __name__ == "__main__":
    main()
