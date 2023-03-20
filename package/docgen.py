import os
import subprocess
import glob


folder_name = os.getcwd().split("/")[-1]  # 获取当前目录的名称

python_files = []  # 用来保存找到的文件

for folder_item in os.listdir("."):
    if os.path.isdir(folder_item):  # 判断是否为文件夹
        for file in os.listdir(folder_item):
            if file.endswith(".py") or file.endswith(".pyi"):  # 判断文件后缀是否为 .py 或者 .pyi
                # 判断文件是否和当前目录名称相同
                if file == folder_item + ".py" or file == folder_item + ".pyi":
                    python_files.append(os.path.join(
                        folder_item, file))  # 将找到的文件添加到列表中


def generate_documentation(path_to_file):
    # Set the output file path based on the input file name
    file_name = os.path.splitext(os.path.basename(path_to_file))[0]
    output_file = f"doc/API_{file_name}.md"

    # Create the 'doc' directory if it does not exist
    if not os.path.exists("doc"):
        os.mkdir("doc")

    # Build the command to run rust-msc-latest-linux
    cmd = ["rust-msc-latest-linux", "--docgen",
           path_to_file, "-o", output_file]

    # Run the command and capture the output
    output = subprocess.run(cmd, capture_output=True, text=True)

    # Print the output
    print(output.stdout)

    # Check if there was an error
    if output.returncode != 0:
        print(output.stderr)


for item in python_files:
    # 调用 rust-msc-latest-linux --docgen <path-to-file.[py/pyi]> -o <file.md>
    generate_documentation(item)


def add_title_to_md_files(directory):
    # 获取目录下所有的md文件路径
    md_files = [f for f in os.listdir(directory) if f.endswith('.md')]

    for md_file in md_files:
        file_path = os.path.join(directory, md_file)
        with open(file_path, 'r') as f:
            lines = f.readlines()

        file_name = os.path.splitext(md_file)[0]
        title = file_name.replace('API_', '')
        # 在文件开头添加一级标题
        lines.insert(0, f'## API\n\n')
        lines.insert(0, f'# {title} 模块 API 文档\n\n')

        with open(file_path, 'w') as f:
            f.writelines(lines)

    print(f'{len(md_files)}个.md文件已添加标题')


# 指定目录路径
directory = 'doc'
add_title_to_md_files(directory)

# 获取目录下所有的md文件路径
md_files = glob.glob(os.path.join(directory, '*.md'))


def remove_prefix(filename):  # 如果文件名以“API_”开头，则去掉该前缀
    if os.path.basename(filename).startswith("API_"):
        return os.path.basename(filename)[4:]  # 否则返回原文件名 else: return filename


md_files_sort = sorted(md_files, key=lambda x: (remove_prefix(x).casefold()))
print(md_files_sort)

# 生成toctree指令内容
toctree_content = ''
for md_file in md_files_sort:
    # 获取文件名（不包含扩展名）
    file_name = os.path.splitext(os.path.basename(md_file))[0]
    # 生成toctree指令项
    toctree_item = f'   {file_name}\n'
    toctree_content += toctree_item

# 生成API文档文件内容
doc_content = f'''模块 API 文档
============================

.. toctree::
   :maxdepth: 1

{toctree_content}
'''

# 写入API文档文件
with open('doc/index_api.rst', 'w') as f:
    f.write(doc_content)

# 获得 module list
module_list = [remove_prefix(md_file).replace('.md', '')
               for md_file in md_files]

print(module_list)

# 收集 ../examples/<module> 下的所有py文件
for module in module_list:
    if module == 'PikaStdDevice':
        module_example = 'Device'
    elif module == 'pika_lvgl':
        module_example = 'lvgl'
    elif module == 'pika_cjson':
        module_example = 'cJSON'
    elif module == 'PikaStdLib':
        module_example = 'BuiltIn'
    else:
        module_example = module
    example_list = glob.glob(f'../examples/{module_example}/*.py')
    print(example_list)
    with open(f'doc/API_{module}.md', 'a') as f:
        f.write('\n\n## Examples\n\n')
        for example in example_list:
            with open(example, 'r') as example_file:
                f.write(f'### {os.path.basename(example)}\n\n')
                f.write('```python\n')
                f.write(example_file.read())
                f.write('\n```\n')
