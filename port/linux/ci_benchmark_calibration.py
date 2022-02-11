#!/usr/bin/python3
import json
from platform import platform
import sys
import os
import fcntl

benchmark_result_file_path = sys.argv[1]

with open(benchmark_result_file_path, 'r', encoding='utf8') as json_in:
    json_data = dict(json.load(json_in))
    benchmarks_data = list(json_data['benchmarks'])

c_base_real_time = benchmarks_data[-1]['cpu_time']
c_base_cali_time = 0.005
cali_ratio = c_base_cali_time/c_base_real_time

platform_cali_ratio = 1

if abs(cali_ratio - 2.005326432941315) < 0.01:
    platform_cali_ratio = 1.1549

print('c_base_real_time:', c_base_real_time)
print('c_base_cali_time:', c_base_cali_time)
print('cali_ratio:', cali_ratio)

for i in range(len(benchmarks_data)):
    benchmarks_data[i]['cpu_time'] *= cali_ratio
    benchmarks_data[i]['real_time'] *= cali_ratio
    benchmarks_data[i]['family_index'] += 1

# new a banchmark
benchmarks_data.insert(0, benchmarks_data[0].copy())
performance_point_name = 'Performance Points'
performance_point_res = benchmarks_data[-1]['cpu_time'] / \
    benchmarks_data[-2]['cpu_time'] * 100 * 100000 * platform_cali_ratio
benchmarks_data[0]['name'] = performance_point_name
benchmarks_data[0]['run_name'] = performance_point_name
benchmarks_data[0]['family_index'] = 0
benchmarks_data[0]['repetitions'] = 1
benchmarks_data[0]['iterations'] = 1
benchmarks_data[0]['real_time'] = performance_point_res
benchmarks_data[0]['cpu_time'] = performance_point_res
benchmarks_data[0]['time_unit'] = 'Point'

print('---------------------------------------------')
print('Perfomance point:', int(performance_point_res), '\n')

# update json_data
json_data['benchmarks'] = benchmarks_data

# save json
with open(benchmark_result_file_path, 'w') as benchmark_reqult_file:
    json.dump(json_data, benchmark_reqult_file)

lock_file_path = 'performance_data.lock'

# save performance_data
with open('performance_data.json', 'r') as perf_json_file:
    # lock
    fcntl.flock(perf_json_file.fileno(), fcntl.LOCK_EX)
    perf_json_data: list = json.load(perf_json_file)
    with open('performance_data.json', 'w') as perf_json_file:
        perf_json_data.append(performance_point_res)
        json.dump(perf_json_data, perf_json_file)
