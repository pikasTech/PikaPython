#!/bin/sh
sh only_make.sh

# 初始化参数变量
tool_option=""
valgrind_common_options=""
gtest_filter=""
massif=false

# 处理命令行参数
for arg in "$@"
do
    case $arg in
        --massif)
            tool_option="--tool=massif --stacks=yes"
            massif=true
            shift
            ;;
        *)
            gtest_filter="--gtest_filter=$arg"
            shift
            ;;
    esac
done

# 设置 valgrind 命令的公共参数
if [ "$massif" = true ]; then
    valgrind_common_options=""
else
    valgrind_common_options="-s --track-origins=yes --leak-check=full --show-leak-kinds=all --exit-on-first-error=yes --error-exitcode=1 --num-callers=50"
fi

# 执行 valgrind 命令
valgrind $tool_option $valgrind_common_options build/test/pikascript_test $gtest_filter
