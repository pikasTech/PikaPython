echo write benchmark result to $1
Release/benchmark/pikascript_benchmark --benchmark_format=json | tee $1 
