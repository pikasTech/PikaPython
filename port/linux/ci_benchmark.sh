Release/benchmark/pikascript_benchmark --benchmark_format=json --benchmark_min_time=10 | tee benchmark_result.json 
python3 ci_benchmark_calibration.py
