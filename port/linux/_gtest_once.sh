set -e

rm -f build/test/pikascript_test
sh make.sh
PIKA_GTEST_FILTER=${PIKA_GTEST_FILTER:--mqtt.*:network.*:socket.*:requests.*:jrpc.BlockingRequestBetweenTwoJRPC:threading.lock_rlock:pikaMain.REPL_push_mode*:pikaMain.runchar_multiline:pikaMain.callback_run_char:pikaMain.REPL_backspace:pikaMain.REPL_tab_completion:pikaMain.REPL_backspace_issue_1:pikaMain.REPL_key_*:pikaMain.SHELL_filter_bye_pika_nomache:pikaMain.shell_nolib_print_help}
timeout 10m build/test/pikascript_test --gtest_filter="$PIKA_GTEST_FILTER"
