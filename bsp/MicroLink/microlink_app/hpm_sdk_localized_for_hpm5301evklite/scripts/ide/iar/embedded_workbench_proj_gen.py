# Copyright 2023 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

#!/usr/bin/env python3

# Usage: python embedded_workbench_proj_gen.py <target_configuration>.json <output_directory>

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

HPM_SDK_BASE = "$HPM_SDK_BASE$"
PROJECT_TEMPLATE = "embedded_workbench_proj_tmpl.xml"
PROJECT_DEBUG_TEMPLATE = "embedded_workbench_debug_tmpl.xml"
HELPER = ProjGenHelper(HPM_SDK_BASE)

def tree_insert_dirs(iar_dir, iar_file, f, tree):
    dirs = iar_dir.split('/')
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
    iar_dir = re.sub(re.escape(d + r'/'), '', iar_dir)
    tree_insert_dirs(iar_dir, iar_file, f, tree[d])

def update_file_tree(sdk_base, out_dir, iar_file, f, tree, use_outdir_relpath):
    iar_dir, file = os.path.split(iar_file)
    if not use_outdir_relpath:
        iar_dir = re.sub(re.escape("%s" % HPM_SDK_BASE) + r'/', '', iar_dir)
    else:
        # remove prefix first
        tmp_dir = re.sub(r'\$PROJ_DIR\$[\\/]', '', iar_dir)
        # get sdk relpath to out_dir
        sdk_base_relpath = re.sub(r'\\', r'/', HELPER.get_relpath(sdk_base, out_dir))
        iar_dir = re.sub(sdk_base_relpath + r'[\\/]?', '', tmp_dir)
    tree_insert_dirs(iar_dir, iar_file, f, tree)

def populate_file_nodes(root, sdk_base, project_dir, custom_board_dir, out_dir, level = 1, use_outdir_relpath = True):
    node = ""
    for n in root.keys():
        if n == "files":
            for f in root[n]:
                if HELPER.is_sdk_file(f, sdk_base):
                    node += "%s<file><name>%s</name>" % (" " * (level * 2), HELPER.get_file_path(f, sdk_base, out_dir, use_outdir_relpath, "$PROJ_DIR$"))
                    obj = re.sub(re.escape(sdk_base + r'[\\/]?'), r'', f)
                elif custom_board_dir is not None and HELPER.is_custom_board_file(f, custom_board_dir):
                    node += "%s<file><name>%s</name>" % (" " * (level * 2), f)
                    f = re.sub(r'^\w:', '', f) # remove windows drive letter
                    t = f.split(custom_board_dir)[1] # get any characters after custom board name
                    t = re.sub(r'^\W', '', t) # remove leading path separator
                    obj = os.path.join(custom_board_dir, t)
                else:
                    node += "%s<file><name>%s</name>" % (" " * (level * 2), f)
                    obj = re.sub(re.escape(project_dir), r'app', f)
                    if obj == f:
                        # deal with drive letter of Windows path
                        obj = re.sub(r'^\w:', '', f)
                obj = re.sub(r'\\', r'/', obj)
                #node += "%s<configuration Name=\"Common\" build_object_file_name=\"%s$(OBJ)\"/>\n" % (" " * ((level + 1) * 2), "/".join(["$(IntDir)", obj]))
                node += "</file>\n"
        else:
            node += "%s<group><name>%s</name>\n" % (" " * (level * 2), n)
            node += populate_file_nodes(root[n], sdk_base, project_dir, custom_board_dir, out_dir, level + 1, use_outdir_relpath = use_outdir_relpath)
            node += "%s</group>\n" % (" " * (level * 2))
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
                iar_file = re.sub(r'\\', r'/', HELPER.get_file_path(f, sdk_base, out_dir, False))
                t = os.path.join(HPM_SDK_BASE, "samples")
                t = re.sub(r'\\', r'/', t)  # convert windows path separator
                iar_file = re.sub(re.escape(t), 'sdk_sample', iar_file)
            elif HELPER.is_sdk_unittest_file(f, sdk_base):
                iar_file = re.sub(r'\\', r'/', HELPER.get_file_path(f, sdk_base, out_dir, False))
                t = os.path.join(HPM_SDK_BASE, "unit_test")
                t = re.sub(r'\\', r'/', t)  # convert windows path separator
                iar_file = re.sub(re.escape(t), 'sdk_unittest', iar_file)
            else:
                iar_file = re.sub(r'\\', r'/', HELPER.get_file_path(f, sdk_base, out_dir, use_outdir_relpath, "$PROJ_DIR$"))
        elif HELPER.is_custom_board_file(f, board_dir):
            if custom_board_dir is None:
                custom_board_dir = board_name
            iar_file = re.sub(board_dir, "boards/" + board_name, f)
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
            iar_file = re.sub(r'^', r'application', m)
        update_file_tree(sdk_base, out_dir, iar_file, f, f_tree, use_outdir_relpath)

    # generate iar project xml content
    nodes = populate_file_nodes(f_tree, sdk_base, app_common_dir, custom_board_dir, out_dir, level = 1, use_outdir_relpath = use_outdir_relpath)
    return nodes

def process_opt_level(config):
    gcc_level = config["target"]["gcc_opt_level"].strip()
    config["target"]["iar_opt_strategy"] = HELPER.dict_get_key_value_with_default(config["target"], "iar_opt_strategy", 2)
    config["target"]["iar_opt_level_slave"] = HELPER.dict_get_key_value_with_default(config["target"], "iar_opt_level_slave", 2)
    config["target"]["iar_opt_no_size_const"] = HELPER.dict_get_key_value_with_default(config["target"], "iar_opt_no_size_const", 0)
    if gcc_level == "-O2":
        config["target"]["iar_opt_level"] = 2
    elif gcc_level == "-O3":
        config["target"]["iar_opt_level"] = 3
    elif gcc_level == "-O1":
        config["target"]["iar_opt_level"] = 1
    else:
        config["target"]["iar_opt_level"] = HELPER.dict_get_key_value_with_default(config["target"], "iar_opt_level", 1)

def process_extra_options(config):
    opts = []
    for o in config["target"]["extra_iar_options"].split(","):
        o = o.strip()
        if not len(o):
            continue
        opts.append(o)

    config["target"]["extra_iar_options"] = opts

def process_cc_lang(config):
    cpp_enabled = HELPER.dict_get_key_value_with_default(config["target"], "cplusplus", 0)
    config["target"]["iar_cc_lang"] = 2
    if cpp_enabled != 0:
        config["target"]["iar_runtime_lib"] = 3 # 0. None, 1. Normal, 2. Full, 3. libc++, 4. custom
    else:
        config["target"]["iar_runtime_lib"] = 1

def generate_iar_project(config, out_dir=".", project_dir = None):
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

    config["target"]["includes"] = HELPER.get_include_path(config, sdk_base, out_dir, [], use_outdir_relpath, "$PROJ_DIR$")
    config["target"]["defines"] = HELPER.get_definitions(config["target"]["defines"])
    config["target"]["file_structure"] = generate_file_structure(files, sdk_base, out_dir, project_dir, board_dir, board_name, use_outdir_relpath)
    config["target"]["iar_link_symbols"] = HELPER.get_link_symbols(config["target"]["link_symbols"])
    config["target"]["linker"] = HELPER.get_file_path(config["target"]["linker"], sdk_base, out_dir, use_outdir_relpath, "$PROJ_DIR$")
    config["target"]["register_definition"] = HELPER.get_file_path(config["target"]["register_definition"], sdk_base, out_dir, use_outdir_relpath, "$PROJ_DIR$")
    config["target"]["cpu_register_definition"] = HELPER.get_file_path(config["target"]["cpu_register_definition"], sdk_base, out_dir, use_outdir_relpath, "$PROJ_DIR$")
    if len(config["target"]["iar_link_input"]):
        libs = []
        for lib in config["target"]["iar_link_input"].split(";"):
            libs.append(HELPER.get_file_path(lib.strip().replace('\\', '/'), sdk_base, out_dir, use_outdir_relpath, "$PROJ_DIR$"))
        config["target"]["iar_link_input"] = libs

    process_opt_level(config)
    process_extra_options(config)
    process_cc_lang(config)
    s = ""
    with open(os.path.join(sdk_base, "scripts", "ide", "iar", PROJECT_TEMPLATE), "r") as f:
        s = f.read()

    t = jinja2.Template(s)
    s = t.render(config)

    return s

def generate_iar_argvars(sdk_base):
    contents = ['<?xml version="1.0" encoding="UTF-8"?>',
                '<iarUserArgVars>',
                '<group name="HPM_SDK" active="true">',
                '<variable>',
                '<name>HPM_SDK_BASE</name>',
                '<value>%s</value>' % sdk_base,
                '</variable>',
                '</group>',
                '</iarUserArgVars>']
    return '\n'.join(contents)

def generate_iar_workspace(project_file_name):
    contents = ['<?xml version="1.0" encoding="UTF-8"?>',
                '<workspace>',
                '<project>',
                '<path>$WS_DIR$\\%s</path>' % project_file_name,
                '</project>',
                '<batchBuild />',
                '</workspace>']
    return '\n'.join(contents)

def update_iar_mulitcore_config(config):
    cmakelists = os.path.join(config["target"]["linked_project_path"], "CMakeLists.txt")
    if not os.path.exists(cmakelists):
        print("!! IAR Embedded workbench: can't find partner project CMakeLists.txt, %s" % cmakelists)
        return config

    p = re.compile(r"^project\((.*)\)")
    project_name = None
    with open(cmakelists, mode = "r", encoding="utf-8") as f:
        for line in f:
            m = re.match(p, line)
            if m is not None:
                project_name = m.group(1)

    if project_name is None:
        print("!! IAR Embedded workbench: can't find project name in partner project CMakeLists.txt, %s" % cmakelists)
        return config


    config["target"]["partner1_workspace"] = os.path.join(config["target"]["linked_project_path"])
    # FIXME: hard coded path needs to be fixed
    config["target"]["partner1_workspace"] = os.path.join(config["target"]["partner1_workspace"], "build", "iar_embedded_workbench", project_name + ".eww")
    config["target"]["partner1_project_name"] = project_name
    return config

def check_fpu_setting(config):
    p = re.compile("[dg]")
    m = re.search(p, config["target"]["compiler_arch"])
    if m is not None:
        config["target"]["fpu_type"] = "2"
        return
    p = re.compile("f")
    m = re.search(p, config["target"]["compiler_arch"])
    if m is not None:
        config["target"]["fpu_type"] = "1"
        return
    config["target"]["fpu_type"] = "0"

def generate_iar_debug(sdk_base, debug_config):
    check_fpu_setting(debug_config)
    if "linked_project_name" in debug_config["target"].keys() and len(debug_config["target"]["linked_project_name"].strip()) > 0:
        config = update_iar_mulitcore_config(debug_config)
    else:
        config = debug_config

    with open(os.path.join(sdk_base, "scripts", "ide", "iar", PROJECT_DEBUG_TEMPLATE), "r") as f:
        s = f.read()

    t = jinja2.Template(s)
    s = t.render(config)

    return s

def main():
    input_file = sys.argv[1]
    out_dir = sys.argv[2]
    project_dir = sys.argv[3]
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    config = HELPER.load_config(input_file)
    sdk_base = HELPER.fix_path(config["target"]["sdk_base"])

    iar_project = generate_iar_project(config, out_dir, project_dir)
    output_filename = config["target"]["name"].replace(".", "_")
    project_file = os.path.join(out_dir, output_filename + ".ewp")
    with open(project_file, "w") as f:
        f.write(iar_project)
    f.close()

    argvars = generate_iar_argvars(sdk_base)
    argvars_file = os.path.join(out_dir, output_filename + ".custom_argvars")
    with open(argvars_file, "w") as f:
        f.write(argvars)
    f.close()

    workspace = generate_iar_workspace(output_filename + ".ewp")
    workspace_file = os.path.join(out_dir, output_filename + ".eww")
    with open(workspace_file, "w") as f:
        f.write(workspace)
    f.close()

    iar_debug = generate_iar_debug(sdk_base, config)
    debug_file = os.path.join(out_dir, output_filename + ".ewd")
    with open(debug_file, "w") as f:
        f.write(iar_debug)
    f.close()

    dep_file = os.path.join(out_dir, output_filename + ".dep")
    if os.path.exists(dep_file):
        os.unlink(dep_file)

    print("-- IAR Embedded Workbench Project: " + project_file)

if __name__ == "__main__":
    main()
