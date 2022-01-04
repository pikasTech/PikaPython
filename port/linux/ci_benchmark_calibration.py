#!/usr/bin/python3
import json
with open('benchmark_result.json', 'r', encoding='utf8') as json_in:
    json_data = dict(json.load(json_in))
    benchmarks_data = list(json_data['benchmarks'])

c_base_real_time = benchmarks_data[-1]['cpu_time']
c_base_cali_time = 0.005
cali_ratio = c_base_cali_time/c_base_real_time
print('c_base_real_time:', c_base_real_time)
print('c_base_cali_time:', c_base_cali_time)
print('cali_ratio:', cali_ratio)

for i in range(len(benchmarks_data)):
    benchmarks_data[i]['cpu_time'] *= cali_ratio
    benchmarks_data[i]['real_time'] *= cali_ratio

for benchmark_item in benchmarks_data:
    benchmark_item = dict(benchmark_item)

with open('benchmark_result.json', 'w') as json_out:
    json.dump(json_data, json_out)