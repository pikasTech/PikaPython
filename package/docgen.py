import os
import subprocess

folder_name = os.getcwd().split("/")[-1] # 获取当前目录的名称

python_files = [] # 用来保存找到的文件

for folder_item in os.listdir("."):
    if os.path.isdir(folder_item): # 判断是否为文件夹
        for file in os.listdir(folder_item):
            if file.endswith(".py") or file.endswith(".pyi"): # 判断文件后缀是否为 .py 或者 .pyi
                # 判断文件是否和当前目录名称相同
                if file == folder_item + ".py" or file == folder_item + ".pyi":
                    python_files.append(os.path.join(folder_item, file)) # 将找到的文件添加到列表中

def generate_documentation(path_to_file):
    # Set the output file path based on the input file name
    file_name = os.path.splitext(os.path.basename(path_to_file))[0]
    output_file = f"doc/API_{file_name}.md"

    # Create the 'doc' directory if it does not exist
    if not os.path.exists("doc"):
        os.mkdir("doc")

    # Build the command to run rust-msc-latest-linux
    cmd = ["rust-msc-latest-linux", "--docgen", path_to_file, "-o", output_file]

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

