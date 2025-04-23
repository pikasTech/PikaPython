# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import re
import sys
import json
import glob
import argparse
import tempfile
import fnmatch
import shutil
import subprocess

sys.path.append(os.path.dirname(sys.argv[0]))
import get_app_info

APP_YAML_NAME = "app.yaml"
DST_SDK_DIR_PREFIX = "hpm_sdk_localized"
localized_cmakelist_bak_surfix = ".localized.bak"
unlocalized_cmakelist_bak_surfix = ".unlocalized.bak"
sdk_base_env_name = "HPM_SDK_BASE"
addition_files_to_be_copied = ["LICENSE", "VERSION", os.path.join("boards", "CMakeLists.txt"), os.path.join("soc", "CMakeLists.txt"), "hpm_sdk_version.h.in", os.path.join("boards", "openocd")]
cmake_file_api_dir = '.cmake/api/v1'
cmake_file_api_query = os.path.join(cmake_file_api_dir, 'query')
cmake_file_api_reply = os.path.join(cmake_file_api_dir, 'reply')
cmake_file_api_target_json_key = 'target'
cmake_file_api_codemodel_json_key = 'codemodel-v2'
parser = argparse.ArgumentParser()
parser.add_argument('--src', help='specify src hpm_sdk app dir to be localized, in which CMakeLists.txt is expected')
parser.add_argument('--bld', help='specify build dir to be locate cmake file apis')
parser.add_argument('--dst', help='specify dst dir to localize hpm_sdk file needed by current project')
parser.add_argument('--sdk', help='specify HPM_SDK_BASE, if not presented, it will take it from environment variables')
parser.add_argument('--brd', help='specify board')
parser.add_argument('--gnt', help='specify generator')
parser.add_argument('--unlocalize', action='store_true', help='unlocalize sdk')
parser.add_argument('-f', action='store_true', help='project will be localized anyway, no matter it has been localized or not')

class Sdk_File_list:
    def __init__(self):
        self.file_list = {}
        self.file_list["soc"] = [] # soc files are stored separately
        self.file_list["board"] = [] # board files are stored separately
        self.file_list["src"] = []
        self.file_list["inc"] = []
        self.file_list["cmakelists"] = []

    def remove_duplicates(self):
        self.file_list["soc"] = list(set(self.file_list["soc"]))
        self.file_list["board"] = list(set(self.file_list["board"]))
        self.file_list["src"] = list(set(self.file_list["src"]))
        self.file_list["inc"] = list(set(self.file_list["inc"]))
        self.file_list["cmakelists"] = list(set(self.file_list["cmakelists"]))

    def add_soc_file(self, f):
        self.file_list["soc"].append(f)

    def add_board_file(self, f):
        self.file_list["board"].append(f)

    def add_src_file(self, f):
        self.file_list["src"].append(f)

    def add_cmakelist(self, f):
        self.file_list["cmakelists"].append(f)

    def add_inc_path(self, d):
        self.file_list["inc"].append(d)

    def get_soc_files(self):
        return self.file_list["soc"]

    def get_board_files(self):
        return self.file_list["board"]

    def get_src_files(self):
        return self.file_list["src"]

    def get_cmakelists(self):
        return self.file_list["cmakelists"]

    def get_inc_paths(self):
        return self.file_list["inc"]

    def has_valid_src(self):
        return len(self.file_list["src"]) > 0

    def has_valid_inc(self):
        return len(self.file_list["inc"]) > 0

    def merge(self, sdk_file_list):
        self.file_list["soc"] = self.file_list["soc"] + sdk_file_list.get_soc_files()
        self.file_list["src"] = self.file_list["src"] + sdk_file_list.get_src_files()
        self.file_list["board"] = self.file_list["board"] + sdk_file_list.get_board_files()
        self.file_list["inc"] = self.file_list["inc"] + sdk_file_list.get_inc_paths()
        self.file_list["cmakelists"] = self.file_list["cmakelists"] + sdk_file_list.get_cmakelists()
        self.remove_duplicates()

def is_c_or_cpp_source_file(file_name):
    if re.search(r".*\.(?:[cSs]|cc|cpp)$", file_name):
        return True
    return False

def is_local_file(src_dir, file_name):
    if re.search(re.escape(os.path.realpath(src_dir)), os.path.realpath(file_name)):
        return True
    return False

def is_sdk_file(sdk_base, file_name, include_samples = False):
    if re.search(re.escape(sdk_base) + r'[\\/]', file_name):
        if re.search(re.escape(os.path.join(sdk_base, 'samples')), file_name):
            if include_samples:
                return True
            else:
                return False
        else:
            return True
    return False

def is_sdk_soc_file(sdk_base, file_name):
    # soc directory pattern: HPM_SDK_BASE/soc/HPM\w+/HPM\w+
    if re.search(re.escape(os.path.join(sdk_base, 'soc', 'HPM')) + r'\w+[/\\]HPM\w+', file_name):
        return True
    return False

def is_sdk_board_file(sdk_base, file_name):
    t = os.path.join(sdk_base, 'boards', 'hpm')
    if re.search(re.escape(t) + r'\w+', file_name):
        return True
    return False

def get_file_realpath(src_dir, file_path):
    if os.path.isabs(file_path):
        f = os.path.realpath(file_path)
    else:
        f = os.path.realpath(os.path.join(src_dir, file_path))
    return f

def inform_already_localized(dst_dir):
    sys.stderr.write("\nError: this project has been localized already, either remove %s manually or run with -f\n" % dst_dir)

def locate_cmake_file_api_files(sdk_base, src_dir, build_dir, cmake_cmd = None, force = False):
    if not os.path.exists(build_dir):
        sys.stderr.write("\nError: failed to locate given build dir: %s\n" % build_dir)
        return None

    reply_dir = os.path.join(build_dir, cmake_file_api_reply)
    if not os.path.exists(reply_dir) and cmake_cmd is not None:
        query_dir = os.path.join(build_dir, cmake_file_api_query)
        query_entry = os.path.join(build_dir, cmake_file_api_query, "codemodel-v2")
        if not os.path.exists(query_dir):
            os.makedirs(query_dir)
        open(query_entry, "w").close()
    elif cmake_cmd is None or len(cmake_cmd) == 0:
        sys.stderr.write("\nError: failed to locate cmake file api reply dir: %s\n" % reply_dir)
        return None

    if not os.path.exists(reply_dir) or force:
        cmake_list = os.path.join(src_dir, "CMakeLists.txt")
        backup_cmake_list = os.path.join(src_dir, cmake_list + localized_cmakelist_bak_surfix)
        if os.path.exists(backup_cmake_list):
            if not force:
                inform_already_localized(dst_dir)
                return None
            else:
                shutil.move(backup_cmake_list, cmake_list)

        p = subprocess.run(cmake_cmd, stdout = subprocess.DEVNULL)
        retval = p.returncode
        if retval:
            sys.stderr.write("\nError: failed to execute cmake cmd: %s\n" % cmake_cmd)
            return None

    found_codemodel_reply_file = False
    file = ""
    for f in os.listdir(reply_dir):
        if (fnmatch.fnmatch(f, "%s-*.json" % cmake_file_api_codemodel_json_key)):
            if not found_codemodel_reply_file:
                found_codemodel_reply_file = True
                file = f
            else:
                sys.stderr.write("\nError: more than one codemodel-v2 reply files have been found\n")
                return None

    file_list = Sdk_File_list()
    target_jsons = []
    if len(file):
        fp = open(os.path.join(reply_dir, file), 'r')
        codemodel = json.load(fp)
        for target in codemodel["configurations"][0]["targets"]:
            target_jsons.append(os.path.join(reply_dir, target["jsonFile"]))
        for d in codemodel["configurations"][0]["directories"]:
            cmakefile = os.path.realpath(os.path.join(d["source"], "CMakeLists.txt"))
            if os.path.exists(cmakefile) and is_sdk_file(sdk_base, cmakefile):
                file_list.add_cmakelist(cmakefile)
        fp.close()
    else:
        sys.stderr.write("\nError: can not locate codemodel-v2 reply file\n")
        return None

    file_list.remove_duplicates()

    for tar in target_jsons:
        fp = open(tar, "r")
        j = json.load(fp)
        for src in j["sources"]:
            # locate and grab source files
            f = get_file_realpath(src_dir, src["path"])
            if is_sdk_file(sdk_base, f) and is_c_or_cpp_source_file(f):
                if is_sdk_soc_file(sdk_base, f):
                    file_list.add_soc_file(f)
                elif is_sdk_board_file(sdk_base, f):
                    file_list.add_board_file(f)
                else:
                    if not is_local_file(src_dir, f):
                        file_list.add_src_file(f)
        if "compileGroups" in j:
            # locate include path
            for cg in j["compileGroups"]:
                for inc in cg["includes"]:
                    d = os.path.realpath(inc["path"])
                    if is_sdk_file(sdk_base, d):
                        if is_sdk_soc_file(sdk_base, d):
                            file_list.add_soc_file(d)
                        else:
                            file_list.add_inc_path(d)
                            if os.path.exists(os.path.join(d, "CMakeLists.txt")):
                                # if CMakeLists.txt is presented, it needs to be processed later
                                file_list.add_cmakelist(os.path.join(d, "CMakeLists.txt"))
        for file in j["backtraceGraph"]["files"]:
            # locate included CMakeLists.txt
            f = get_file_realpath(src_dir, file)
            if is_sdk_file(sdk_base, f) and not re.search(re.escape(os.path.join(sdk_base, 'cmake')), f):
                tmp = os.path.dirname(f)
                while not sdk_base == tmp:
                    parent_cmakelist = os.path.join(tmp, "CMakeLists.txt")
                    if os.path.exists(parent_cmakelist):
                        file_list.add_cmakelist(parent_cmakelist)
                    tmp = os.path.dirname(tmp)
                file_list.add_cmakelist(f)

        fp.close()

    # remove dups
    file_list.remove_duplicates()

    return file_list

def copy_sdk_files(sdk_base, src_file, dst_top):
    src = re.sub(re.escape(sdk_base) + r"[/\\]", '', src_file)
    dst = os.path.join(dst_top, src)
    if not os.path.exists(os.path.dirname(dst)):
        os.makedirs(os.path.dirname(dst))
    shutil.copy2(src_file, dst)

def copy_sdk_directories(sdk_base, src_dir, dst_top, file_filter = None):
    dst_dir = os.path.join(dst_top, re.sub(re.escape(sdk_base) + r"[/\\]", '', src_dir))
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)
        # print("creating %s" % os.path.dirname(dst_dir))
    for file in os.listdir(src_dir):
        src = os.path.join(src_dir, file)
        if not os.path.isdir(src):
            if file_filter is None:
                # print("copying %s to %s" % (file, dst_dir))
                shutil.copy2(src, dst_dir)
            else:
                if (fnmatch.fnmatch(file, file_filter)):
                    # print("filter(%s): copying %s to %s" % (file_filter, file, dst_dir))
                    shutil.copy2(src, dst_dir)
        else:
            copy_sdk_directories(sdk_base, src, dst_top, file_filter)

def copy_sdk_soc_board_files(sdk_base, dst_dir, file_list):
    soc_src = None
    soc_path = None
    board_src = None
    board_path = None
    for f in file_list.get_soc_files():
        if is_sdk_soc_file(sdk_base, f):
            # soc directory pattern: HPM_SDK_BASE/soc/HPM\w+/HPM\w+
            soc_src = re.sub(r'(' + re.escape(sdk_base) + r'[/\\]soc[/\\]HPM\w+[/\\]HPM\w+)[/\\].*', r'\1', f)
            soc_path = re.sub(re.escape(sdk_base) + r'[/\\]', '', f)
            soc_path = re.sub(r'(soc[/\\]HPM\w+[/\\]HPM\w+)[/\\].*', r'\1', soc_path)
            break
    if soc_path is None:
        sys.stderr.write("\nError: no soc path is found\n")
        return False
    # all soc file needs to be copied
    dst_soc_path = os.path.join(dst_dir, soc_path)
    if not os.path.exists(os.path.dirname(dst_soc_path)):
        os.makedirs(os.path.dirname(dst_soc_path))
    shutil.copytree(soc_src, dst_soc_path)

    for f in file_list.get_board_files():
        m = re.match(re.escape(os.path.join(sdk_base, 'boards')) + r'\W(hpm\w+)[/\\].*$', f)
        if m:
            board_name = m.group(1)
            board_src = os.path.join(sdk_base, "boards", board_name)
            if os.path.exists(os.path.join(board_src, board_name + ".yaml")):
                board_path = os.path.join("boards", board_name)
                break

    if board_path is not None:
        # all sdk board files need to be copied
        os.makedirs(os.path.join(dst_dir, "boards"))
        dst_board_path = os.path.join(dst_dir, board_path)
        shutil.copytree(board_src, dst_board_path)

    return True

def copy_additional_sdk_files(sdk_base, dst_dir):
    dst_script_path = os.path.join(dst_dir, "scripts")
    shutil.copytree(os.path.join(sdk_base, "scripts"), dst_script_path)
    dst_script_path = os.path.join(dst_dir, "cmake")
    shutil.copytree(os.path.join(sdk_base, "cmake"), dst_script_path)

    for f in addition_files_to_be_copied:
        src_path = os.path.join(sdk_base, f)
        dst_path = os.path.join(dst_dir, f)
        if os.path.exists(os.path.join(sdk_base, f)):
            dir_path = os.path.dirname(dst_path)
            if not os.path.exists(dir_path):
                os.makedirs(dir_path)
            if os.path.isdir(src_path):
                shutil.copytree(src_path, dst_path)
            else:
                shutil.copy2(src_path, dst_path)

def create_cmakelist_hierarchy(sdk_base, dst_dir, cmakelist):
    has_libraries = False
    fp = open(cmakelist, "r")
    for line in fp.readlines():
        m = re.search(r'add_subdirectory\((.*)\)', line)
        if m:
            target = m.group(1)
            if re.match(r'(?:soc|boards)', target):
                continue
            target_src_path = os.path.join(os.path.dirname(cmakelist), target)
            target_path = os.path.join(dst_dir, re.sub(re.escape(sdk_base) + r'[/\\]?', '', os.path.dirname(cmakelist)), target)
            src = os.path.join(target_src_path, "CMakeLists.txt")
            if re.search(r'\$\{.*\}', src):
                # any path using cmake variables will be skipped, since it will be copied directly according to json file
                continue
            dst = os.path.join(target_path, "CMakeLists.txt")
            if not os.path.exists(target_path):
                os.makedirs(target_path)
            shutil.copy2(src, dst)
            create_cmakelist_hierarchy(sdk_base, dst_dir, src)
        else:
            m = re.search(r'sdk_inc\((.*)\)', line)
            if m:
                inc = m.group(1)
                if re.match(r'\${.*}', inc):
                    continue
                inc_src_path = os.path.join(os.path.dirname(cmakelist), inc)
                inc_dst_path = os.path.join(dst_dir, re.sub(re.escape(sdk_base) + r'[/\\]?', '', os.path.dirname(cmakelist)))
                for hdr in os.listdir(inc_src_path):
                    src = os.path.join(inc_src_path, hdr)
                    if fnmatch.fnmatch(hdr, "*.h"):
                        dst = os.path.join(inc_dst_path, inc)
                        shutil.copy2(src, dst)
            else:
                #  Deal with libraries files, since library might not be linked gcc,
                # so localize all libraries, if any library link function is presented
                # in current CMakeList.txt.
                m = re.search(r'(sdk_\w+_ld_lib|sdk_link_libraries)', line)
                if m:
                    has_libraries = True
    fp.close()

    # process headers locating at the same path of current CMakeLists.txt
    cwd = os.path.dirname(cmakelist)
    for hdr in os.listdir(cwd):
        src = os.path.join(cwd, hdr)
        inc_dst_path = os.path.join(dst_dir, re.sub(re.escape(sdk_base) + r'[/\\]?', '', os.path.dirname(cmakelist)))
        if fnmatch.fnmatch(hdr, "*.h"):
            dst = os.path.join(inc_dst_path, hdr)
            shutil.copy2(src, dst)

    if has_libraries:
        for f in glob.glob(os.path.join(cwd, "**", "*.a"), recursive = True):
            copy_sdk_files(sdk_base, f, dst_dir)

def copy_sdk_cmakelists(sdk_base, dst_dir, cmakelists):
    for cl in cmakelists:
        # copy file first
        copy_sdk_files(sdk_base, cl, dst_dir)
        # check if any "add_subdirectory" is presented, or these sub dirs need to be taken care of
        create_cmakelist_hierarchy(sdk_base, dst_dir, cl)


def update_cmakelists(src_dir, dst_dir, dst_sdk_dir_name, board_name, generator, build_dir, dst_sdk_base_use_relpath = False):
    cmake_list = os.path.join(src_dir, "CMakeLists.txt")
    if not cmake_list:
        sys.stderr.write("\nError: can not locate application CMakeLists.txt in %s\n" % src_dir)
        return False

    backup_cmake_list = cmake_list + localized_cmakelist_bak_surfix
    if os.path.exists(backup_cmake_list):
        sys.stderr.write("\nError: localization backup file exists, please remove it: %s" % backup_cmake_list)
        return False

    new_sdk_base = os.path.join(dst_dir, dst_sdk_dir_name)
    if not os.path.exists(new_sdk_base):
        sys.stderr.write("\nError: can not locate localized hpm_sdk in %s\n" % dst_dir)
        return False
    if dst_sdk_base_use_relpath:
        try:
            new_sdk_base = os.path.join("${CMAKE_CURRENT_SOURCE_DIR}", os.path.relpath(new_sdk_base, src_dir))
        except ValueError:
            new_sdk_base = new_sdk_base
    new_sdk_base = re.sub(r'\\', '/', new_sdk_base)

    new_cmake_list = tempfile.mktemp()
    new = open(new_cmake_list, "w", encoding = "utf-8")
    org = open(cmake_list, "r", encoding = "utf-8")

    result = False
    try:
        for line in org.readlines():
            if re.match(r'^\s*find_package\(hpm-sdk ', line):
                new.write("\n")
                new.write("# the following lines before \"### END ###\" are added during project localization\n")
                new.write("### START ###\n")
                new.write("set(ENV{HPM_SDK_BASE} \"%s\")\n" % new_sdk_base)
                new.write("set(LOCALIZED_BOARD \"%s\")\n" % board_name)
                new.write("if(BOARD)\n")
                new.write("  if(NOT ${BOARD} MATCHES ${LOCALIZED_BOARD})\n")
                new.write("    message(FATAL_ERROR \"ABORT:\\n hpm sdk has been localized for ${LOCALIZED_BOARD} already.\")\n")
                new.write("  endif()\n")
                new.write("endif()\n")
                new.write("set(BOARD ${LOCALIZED_BOARD})\n")
                new.write("### END ###\n")
                new.write("\n")
                new.write("\n")
            new.write(line)
        result = True
    except UnicodeDecodeError:
        sys.stderr.write("\nError: Decoding unicode error, update CMakeLists.txt failed\n")

    org.close()
    new.close()

    if not result:
        return False

    shutil.move(cmake_list, backup_cmake_list)
    shutil.move(new_cmake_list, cmake_list)

    cmake_cmd = ["cmake", "-G" + args.gnt, "-B" + build_dir, "-S" + src_dir]
    p = subprocess.run(cmake_cmd)
    if p.returncode:
        sys.stderr.write("\nError: failed to execute cmake cmd: %s\n" % cmake_cmd)
        return False
    return True

def copy_project_files(sdk_base, src_dir, dst_dir, dst_sdk_dir_name, file_list, force = False):
    if not os.path.exists(src_dir):
        sys.stderr.write("\nError: failed to locate src dir: %s\n" % src_dir)
        return False
    if not os.path.exists(dst_dir):
        sys.stderr.write("\nError: failed to locate dst dir: %s\n" % dst_dir)
        return False
    if not file_list.has_valid_src():
        sys.stderr.write("\nError: no hpm_sdk source file is found\n")
        return False
    if not file_list.has_valid_inc():
        sys.stderr.write("\nError: no hpm_sdk include path is found\n")
        return False

    # create localized sdk folder
    dst_sdk_dir = os.path.join(dst_dir, dst_sdk_dir_name)
    if os.path.exists(dst_sdk_dir):
        if not force:
            inform_already_localized(dst_sdk_dir)
            return False
        else:
            shutil.rmtree(dst_sdk_dir)
    os.makedirs(dst_sdk_dir)

    if not copy_sdk_soc_board_files(sdk_base, dst_sdk_dir, file_list):
        return False

    for f in file_list.get_src_files():
        copy_sdk_files(sdk_base, f, dst_sdk_dir)
    for f in file_list.get_inc_paths():
        copy_sdk_directories(sdk_base, f, dst_sdk_dir, file_filter = "*.h")
    copy_sdk_cmakelists(sdk_base, dst_sdk_dir, file_list.get_cmakelists())

    copy_additional_sdk_files(sdk_base, dst_sdk_dir)
    return True

def get_linked_project(app_dir):
    app_yaml = os.path.join(app_dir, APP_YAML_NAME)
    if not os.path.exists(app_yaml):
        return None
    retval, output = get_app_info.get_app_info(app_yaml, get_app_info.APP_INFO_LINKED_PROJECT_PATH, False)
    if not retval:
        return None
    return output

def restore_cmakefile(src_dir):
    current_cmakelist = os.path.join(src_dir, "CMakeLists.txt")
    localized_cmakelist_backup = current_cmakelist + localized_cmakelist_bak_surfix
    if not os.path.exists(localized_cmakelist_backup):
        return True
    unlocalized_cmakelist_backup = current_cmakelist + unlocalized_cmakelist_bak_surfix
    if os.path.exists(unlocalized_cmakelist_backup):
        print("CMakeLists.txt backup file created for unlocalization exists, please check it and make sure all needed changes have been saved.\nIn order to run uninitialize successfully, please remove this backup file (%s)" % unlocalized_cmakelist_backup)
        return False
    shutil.move(current_cmakelist, unlocalized_cmakelist_backup)
    shutil.move(localized_cmakelist_backup, current_cmakelist)

    return True

def unlocalize_sdk(src_dir, board_name, generator, build_dir):
    linked_project_path = get_linked_project(src_dir)
    if linked_project_path is not None:
        if not os.path.isabs(linked_project_path):
            linked_project_path = os.path.realpath(os.path.join(src_dir, linked_project_path))
        if not restore_cmakefile(linked_project_path):
            return False
    if not restore_cmakefile(src_dir):
        return False

    cmake_cmd = ["cmake", "-G" + generator, "-B" + build_dir, "-S" + src_dir, "-DBOARD=" + board_name]
    p = subprocess.run(cmake_cmd)
    if p.returncode:
        sys.stderr.write("\nError: failed to execute cmake cmd: %s\n" % cmake_cmd)
        return False
    return True

def localize_sdk(sdk_base, src_dir, dst_dir, dst_sdk_prefix, board_name, generator, build_dir, force = False):
    if board_name is None:
        sys.stderr.write("\nError: no board is given\n")
        return False
    else:
        dst_sdk_dir_name = ("%s_for_%s" % (dst_sdk_prefix, board_name))

    dir_to_be_removed = []
    if build_dir is None:
        build_dir = tempfile.mkdtemp()
        dir_to_be_removed.append(build_dir)

    if not os.path.exists(build_dir):
        sys.stderr.write("\nError: failed to locate build dir (%s)\n" % build_dir)
        return False

    if dst_dir == src_dir:
        dst_sdk_base_use_relpath = True
    else:
        dst_sdk_base_use_relpath = False
    linked_project_path = get_linked_project(src_dir)
    linked_project_sdk_file_list = None
    if linked_project_path is not None:
        linked_project_build_dir = tempfile.mkdtemp()
        dir_to_be_removed.append(linked_project_build_dir)
        if not os.path.isabs(linked_project_path):
            linked_project_path = os.path.realpath(os.path.join(src_dir, linked_project_path))
        linked_project_cmake_cmd = ["cmake", "-DHPM_SDK_BASE=" + sdk_base, "-G" + generator, "-DBOARD=" + board_name, "-B" + linked_project_build_dir, "-S" + linked_project_path]
        linked_project_sdk_file_list = locate_cmake_file_api_files(sdk_base, linked_project_path, linked_project_build_dir, linked_project_cmake_cmd, force)

    cmake_cmd = ["cmake", "-DHPM_SDK_BASE=" + sdk_base, "-G" + generator, "-DBOARD=" + board_name, "-B" + build_dir, "-S" + src_dir]
    sdk_file_list = locate_cmake_file_api_files(sdk_base, src_dir, build_dir, cmake_cmd, force)

    if linked_project_sdk_file_list is not None:
        sdk_file_list.merge(linked_project_sdk_file_list)

    result = False
    if sdk_file_list is not None and copy_project_files(sdk_base, src_dir, dst_dir, dst_sdk_dir_name, sdk_file_list, force):
        result = True
        if linked_project_path is not None:
            if not update_cmakelists(linked_project_path, dst_dir, dst_sdk_dir_name, board_name, generator, linked_project_build_dir, dst_sdk_base_use_relpath):
                result = False
        if not update_cmakelists(src_dir, dst_dir, dst_sdk_dir_name, board_name, generator, build_dir, dst_sdk_base_use_relpath):
            result = False

    # cleanup temp directories
    for d in dir_to_be_removed:
        shutil.rmtree(d)

    return result

if __name__ == "__main__":
    args = parser.parse_args()
    src_dir = args.src
    if src_dir is None:
        sys.stderr.write("\nError: no source dir is given\n")
        parser.print_help()
        sys.exit(1)
    elif not os.path.exists(src_dir):
        sys.stderr.write("\nError: source dir: \"%s\" can not be found\n" % os.path.realpath(src_dir))
        sys.exit(1)
    else:
        src_dir = os.path.realpath(src_dir)
        if not os.path.exists(os.path.join(src_dir, "CMakeLists.txt")):
            sys.stderr.write("\nError: can not locate CMakeLists.txt in %s\n" % src_dir)
            sys.exit(1)

    result = True
    if not args.unlocalize:
        if args.sdk is None and sdk_base_env_name not in os.environ:
            sys.stderr.write("\nError: no valid HPM_SDK_BASE is given, either set it as environment variable or pass it using --sdk SDK\n")
            sys.exit(1)

        if args.sdk is not None:
            sdk_base = args.sdk
        else:
            sdk_base = os.environ[sdk_base_env_name]

        sdk_base = os.path.realpath(sdk_base)
        if not os.path.exists(sdk_base):
            sys.stderr.write("\nError: HPM_SDK_BASE (%s) can not be located\n" % sdk_base)
            sys.exit(1)
        else:
            os.environ[sdk_base_env_name] = sdk_base

        if args.dst is not None:
            dst_dir = os.path.realpath(args.dst)
        else:
            dst_dir = os.path.realpath(src_dir)

        result = localize_sdk(sdk_base, src_dir, dst_dir, DST_SDK_DIR_PREFIX, args.brd, args.gnt, args.bld, args.f)
    else:
        result = unlocalize_sdk(src_dir, args.brd, args.gnt, args.bld)

    sys.exit(not result)
