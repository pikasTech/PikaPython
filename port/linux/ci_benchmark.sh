sh ci_benchmark_times.sh 2
python3 ci_benchmark_select.py

#clear
rm benchmark_result_*.json
rm performance_data.json
echo 
