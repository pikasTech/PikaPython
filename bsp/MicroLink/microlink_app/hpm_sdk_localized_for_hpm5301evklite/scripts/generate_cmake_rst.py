#!/usr/bin/env python3

# Software License Agreement (BSD License)
#
# Copyright (c) 2012, Willow Garage, Inc.
# Copyright (c) 2024 HPMicro
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of Willow Garage, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from __future__ import print_function

import argparse
import json
import os
import re
import sys

"""Simple superficial API doc generator for .cmake files"""


def crawl_for_cmake(path, excluded_files=None):
    """Crawls over path, looking for files named *.cmake, returns tuple of full and relative path."""
    cmake_files = []
    for (parentdir, dirs, files) in os.walk(path):
        for filename in files:
            if not filename.endswith('.cmake') or \
                    (excluded_files and filename in excluded_files):
                continue
            fullpath = os.path.join(parentdir, filename)
            relpath = os.path.relpath(fullpath, path)
            cmake_files.append((fullpath, relpath))
        for dirname in dirs:
            cmake_files = cmake_files + crawl_for_cmake(dirname)
    return cmake_files


def generate_rst(files, skip_private=False, skip_undocumented=False, headline=None):
    """
    Each of the CMake files is traversed line by line, looking for lines like function(...) or macro(...).

    For each of these, multiple lines of reStructured text are added documenting the function.
    """
    public = {}
    documented = {}
    undocumented = {}
    for (fullpath, relpath) in files:
        last_block = []
        last_block_public = False
        with open(fullpath, 'r') as f:
            lines = f.readlines()
        for line in lines:
            if line.startswith('#'):
                line = line.lstrip('#')
                if line.strip() == '@public':
                    last_block_public = True
                else:
                    last_block.append(line.rstrip('\n'))
            else:
                declaration = re.match(r'[a-zA-Z]+ *\([a-zA-Z0-9_ ]+\)', line)
                if declaration is None:
                    last_block = []
                    last_block_public = False
                else:
                    tokens = line.split('(')
                    dec_type = tokens[0].strip()
                    dec_args = tokens[1].strip().rstrip(')').split(' ')

                    if dec_type == 'function' or dec_type == 'macro':
                        rst = []
                        # directives defined in catkin-sphinx
                        dec_line = '.. _`%s_ref`:\n\n`%s`\n%s\n\n.. cmake:command:: %s(%s)' % (dec_args[0], dec_args[0], '~' * (len(dec_args[0]) + 2), dec_args[0], ', '.join(dec_args[1:]))
                        rst.append(dec_line)
                        rst.append('')
                        rst.append(' *[%s defined in %s]*' % (dec_type, relpath))
                        if last_block:
                            rst.append('')
                            rst.extend(last_block)

                        if dec_args[0] in documented or dec_args[0] in undocumented:
                            raise RuntimeError('Function/macro with same name "%s" exists multiple times' % dec_args[0])
                        if last_block_public:
                            public[dec_args[0]] = rst
                        elif last_block:
                            documented[dec_args[0]] = rst
                        else:
                            undocumented[dec_args[0]] = rst

                    last_block = []
                    last_block_public = False

    if headline is not None:
        rst = [headline]
    else:
        rst = ['Extracted CMake API reference']
    rst.append('=============================')
    rst.append('This page was auto-generated from cmake source files using %s\n' % os.path.basename(__file__))
    rst.append('.. ' + '!' * 70)
    rst.append('.. !!!!!! Auto-generated file, do not modify')
    rst.append('.. ' + '!' * 70)
    rst.append('')
    rst.append('.. contents::')
    rst.append('   :local:')
    rst.append('')
    rst.append('')
    rst.append('Public CMake functions / macros')
    rst.append('-------------------------------')
    rst.append('')
    for name in sorted(public.keys()):
        rst.append(' * :cmake:command:`%s`' % name)
    for name in sorted(public.keys()):
        rst.append('')
        rst.extend(public[name])
    rst.append('')

    if not skip_private:
        rst.append('Non-public CMake functions / macros')
        rst.append('-----------------------------------')
        rst.append('')
        for name in sorted(documented.keys()):
            rst.append(' * :cmake:command:`%s`' % name)
        for name in sorted(documented.keys()):
            rst.append('')
            rst.extend(documented[name])
        rst.append('')

    if not skip_undocumented:
        rst.append('Not documented CMake functions / macros')
        rst.append('---------------------------------------')
        for name in sorted(undocumented.keys()):
            rst.append('')
            rst.extend(undocumented[name])
        rst.append('')

    return rst


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Crawls a path for .cmake files and extract documentation of functions and macros into reStructured text.')
    parser.add_argument('path', nargs='?', default='.', help='The path to be crawled')
    parser.add_argument('-o', '--output', help='The name of the generated rst file')
    parser.add_argument('--skip_private', action='store_true', help='Skip documented items not marked with @public')
    parser.add_argument('--skip_undocumented', action='store_true', help='Skip items without documentation.')
    parser.add_argument('--headline', help='The headline of generated doc.')

    args = parser.parse_args()

    exclusions = '{}/.sphinx_exclusions.json'.format(args.path)
    excluded_files = []
    if os.path.exists(exclusions):
        try:
            with open(exclusions, 'r') as f:
                excluded_files = json.load(f)
        except (TypeError, ValueError) as err:
            print('unable to load exclusions\nerr={}\n'
                  'make sure the file <{}> is valid json or remove it'.
                  format(err, exclusions), file=sys.stderr)
            sys.exit(-1)

    cmake_files = crawl_for_cmake(args.path, excluded_files)
    lines = generate_rst(cmake_files, args.skip_private, args.skip_undocumented, args.headline)
    if args.output:
        with open(args.output, 'w') as f:
            f.write('\n'.join(lines))
    else:
        for line in lines:
            print(line)
    sys.exit(0)
