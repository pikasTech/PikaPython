import os
import sys
import subprocess
from pathlib import Path


def format_files_in_dir(dir_path, dir_skip):
    # 遍历目录及其子目录中的所有文件
    for filepath in dir_path.rglob('*'):
        # Skip if the current file's directory or any of its parents are in dir_skip
        if any(str(filepath).startswith(dir) for dir in dir_skip):
            continue

        # 只处理.c, .h, .cpp文件
        if filepath.suffix in ['.c', '.h', '.cpp']:
            try:
                # 使用clang-format命令格式化文件
                subprocess.run(
                    ['clang-format', '-i', str(filepath)], check=True)
                print(f'Formatted file: {filepath}')
            except subprocess.CalledProcessError as e:
                print(
                    f'Error formatting file: {filepath}. Error message: {str(e)}')

            # 确保文件最后一行是空行
            with open(filepath, 'r+', encoding='utf-8') as file:
                lines = file.readlines()
                if lines and not lines[-1].endswith('\n'):
                    # 如果最后一行不是空行，添加一个
                    file.write('\n')
                    print(f'Added newline to file: {filepath}')


# 从命令行参数获取目录列表
dirs = sys.argv[1:]

# 如果没有命令行参数, 则使用默认目录
if len(dirs) == 0:
    dirs = ['package/pikascript/pikascript-lib',
            'package/pikascript/pikascript-core', 'test']

dir_skip = ['package/pikascript/pikascript-lib/re',
            'package/pikascript/pikascript-lib/PikaNN']

# 对每个目录进行处理
for dir_path_str in dirs:
    dir_path = Path(dir_path_str)
    if dir_path.is_dir():
        format_files_in_dir(dir_path, dir_skip)
        print(f'Finished processing directory: {dir_path_str}')
    else:
        print(f"'{dir_path_str}' is not a directory. Skipping...")
