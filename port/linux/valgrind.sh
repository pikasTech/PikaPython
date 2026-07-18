#!/bin/sh
set -e

sh make.sh

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

if [ -z "$gtest_filter" ]; then
    PIKA_GTEST_FILTER=${PIKA_GTEST_FILTER:--mqtt.*:network.*:socket.*:requests.*:jrpc.BlockingRequestBetweenTwoJRPC:pikaMain.REPL_push_mode*:pikaMain.runchar_multiline:pikaMain.callback_run_char:pikaMain.REPL_backspace:pikaMain.REPL_tab_completion:pikaMain.REPL_backspace_issue_1:pikaMain.REPL_key_*:pikaMain.SHELL_filter_bye_pika_nomache:pikaMain.shell_nolib_print_help}
    gtest_filter="--gtest_filter=$PIKA_GTEST_FILTER"
fi

# 设置 valgrind 命令的公共参数
if [ "$massif" = true ]; then
    valgrind_common_options=""
else
    valgrind_common_options="-s --track-origins=yes --leak-check=full --show-leak-kinds=all --exit-on-first-error=yes --error-exitcode=1 --num-callers=50"
fi

# 执行 valgrind 命令
timeout 10m valgrind $tool_option $valgrind_common_options build/test/pikascript_test $gtest_filter
