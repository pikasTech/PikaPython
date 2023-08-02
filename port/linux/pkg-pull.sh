#!/bin/bash

# 验证参数数量
if [ "$#" -ne 1 ]; then
    echo "错误：需要一个参数"
    echo "用法：$0 <package_name>"
    exit 1
fi

src_dir="../../package/${1}"
dest_dir="package/pikascript/pikascript-lib/${1}"

# 验证源目录是否存在
if [ ! -d "$src_dir" ]; then
    echo "错误：源目录不存在"
    exit 2
fi

# 如果目标目录不存在，创建它
mkdir -p "$dest_dir"

# 把源目录中的文件拷贝到目标目录
cp -r "$src_dir/"* "$dest_dir/"

echo "拷贝完成"

