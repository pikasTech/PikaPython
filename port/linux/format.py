import sys
import subprocess
from pathlib import Path
import codecs


def format_file(filepath):
    try:
        subprocess.run(['clang-format', '-i', str(filepath)], check=True)
    except subprocess.CalledProcessError as e:
        print(f'Error formatting file: {filepath}. Error message: {str(e)}')

    non_ascii_lines = []

    with codecs.open(filepath, 'r', 'utf-8-sig') as file:
        lines = file.readlines()
        for i, line in enumerate(lines):
            if filepath.suffix != '.cpp':
                if not line.isascii():
                    non_ascii_lines.append((i + 1, line))

        if lines and not lines[-1].endswith('\n'):
            lines.append('\n')
            print(f'Added newline to file: {filepath}')

    with codecs.open(filepath, 'w', 'utf-8') as file:
        file.writelines(lines)

    if non_ascii_lines:
        # print with yellow color
        print(f'\033[33mFound non-ASCII lines in file: {filepath}\033[0m')
        for line_no, line in non_ascii_lines:
            print(f'Line {filepath}:{line_no}: {line}')


def format_files_in_dir(dir_path, dir_skip):
    for filepath in dir_path.rglob('*'):
        # Skip if the current file's directory or any of its parents are in dir_skip
        if any(str(filepath).startswith(dir) for dir in dir_skip):
            continue

        # 只处理.c, .h, .cpp文件
        if filepath.suffix in ['.c', '.h', '.cpp']:
            format_file(filepath)


# 从命令行参数获取目录列表
dirs = sys.argv[1:]

# 如果没有命令行参数, 则使用默认目录
if len(dirs) == 0:
    dirs = ['package/pikascript/pikascript-lib',
            'package/pikascript/pikascript-core', 'test']

dir_skip = ['package/pikascript/pikascript-lib/re',
            'package/pikascript/pikascript-lib/PikaNN',
            'package/pikascript/pikascript-lib/modbus',
            'package/pikascript/pikascript-lib/PikaStdDevice/pika_hal_table_rule.h'
            ]

# 对每个目录进行处理
for dir_path_str in dirs:
    dir_path = Path(dir_path_str)
    if dir_path.is_dir():
        format_files_in_dir(dir_path, dir_skip)
        print(f'Finished processing directory: {dir_path_str}')
    else:
        print(f"'{dir_path_str}' is not a directory. Skipping...")
