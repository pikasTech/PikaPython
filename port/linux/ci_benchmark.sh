cp config/pika_config_optimize_speed.h config/pika_config.h 

bash ci_benchmark_times.sh 20
python3 ci_benchmark_select.py

#clear
rm benchmark_result_*.json
rm performance_data.json
echo 

# restore backup
rm config/pika_config.h
