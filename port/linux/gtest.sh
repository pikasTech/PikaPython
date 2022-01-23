rm build/test/pikascript_test
sh make.sh
build/test/pikascript_test 
#build/benchmark/pikascript_benchmark --benchmark_format=json | tee benchmark_result.json
#build/test/pikascript_test --gtest_filter=args_*
