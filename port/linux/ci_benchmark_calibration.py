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
    benchmarks_data[i]['family_index'] += 1

# manel cali rate
manul_cali_rate = (2100/3300)

# new a banchmark
benchmarks_data.insert(0, benchmarks_data[0].copy())
performance_point_name = 'Performance Points'
performance_point_res = benchmarks_data[-1]['cpu_time'] / \
    benchmarks_data[-2]['cpu_time'] * 100 * 100000 * manul_cali_rate
benchmarks_data[0]['name'] = performance_point_name
benchmarks_data[0]['run_name'] = performance_point_name
benchmarks_data[0]['family_index'] = 0
benchmarks_data[0]['repetitions'] = 1
benchmarks_data[0]['iterations'] = 1
benchmarks_data[0]['real_time'] = performance_point_res
benchmarks_data[0]['cpu_time'] = performance_point_res
benchmarks_data[0]['time_unit'] = 'Point'

print('---------------------------------------------')
print('Perfomance point:', int(performance_point_res))

# update json_data
json_data['benchmarks'] = benchmarks_data

# save json
with open('benchmark_result.json', 'w') as json_out:
    json.dump(json_data, json_out)
