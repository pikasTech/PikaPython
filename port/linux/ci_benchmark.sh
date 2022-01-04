build/benchmark/pikascript_benchmark --benchmark_format=json | tee benchmark_result.json 
python3 ci_benchmark_calibration.py
