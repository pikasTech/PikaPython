cp config/pika_config_benchmark.h config/pika_config.h 
bash prof.sh

bash ci_benchmark_times.sh 20
python3 ci_benchmark_select.py

#clear
rm benchmark_result_*.json
rm performance_data.json
echo 

# restore backup
cp config/pika_config_default.h config/pika_config.h 
