rm build/test/pikascript_test
sh make.sh
build/test/pikascript_test 
build/benchmark/pikascript_benchmark --benchmark_format=json
#build/test/pikascript_test --gtest_filter=args_*
