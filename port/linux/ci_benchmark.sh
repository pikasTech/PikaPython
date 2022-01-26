cd Release && make -j
cd ..
bash ci_benchmark_run.sh
python3 ci_benchmark_calibration.py
