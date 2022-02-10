#! /usr/bin/python3
import json
import matplotlib.pyplot as plt
import numpy as np

data_js_prefix = "window.BENCHMARK_DATA = "

def get_data_json(path:str):
    data_file = open(path,'r')
    data_json_str = data_file.read().replace(data_js_prefix, "")
    return json.loads(data_json_str)


def get_benchmark_list(path:str)->list:
    data_json = get_data_json(path)
    return data_json['entries']['Benchmark']


def get_perferance_point_list(path:str)->list:
    benchmark_list = get_benchmark_list(path)
    perferance_point_list:list = []
    for i in range(benchmark_list.__len__()):
        benchmark = benchmark_list[i]
        benches:list = benchmark['benches']
        # for j in range(0, benches.__len__()):
        perferance_point_list.append(benches[0]['value'])
    return perferance_point_list

p_list = get_perferance_point_list('data.js')

median_range = 10

p_list_after = []

# add head
for i in range(0, median_range//2):
    p_list_after.append(p_list[i])

# median filter
for i in range(median_range//2, p_list.__len__() - median_range//2):
    # get p_nearby
    p_nearby = []
    for j in range(median_range):
        p_nearby.append(p_list[i - median_range//2 + j])
    p_after = np.median(p_nearby)
    p_list_after.append(p_after)

# add end
for i in range(p_list.__len__() - median_range//2, p_list.__len__()):
    p_list_after.append(p_list[i])

def set_data_json_after(p_list_after:list):
    data_json = get_data_json('data.js')
    benchmark_list = get_benchmark_list('data.js')
    for i in range(benchmark_list.__len__()):
        data_json['entries']['Benchmark'][i]['benches'][0]['value'] = p_list_after[i]
    data_json_after_str = json.dumps(data_json)
    data_json_after_str = data_js_prefix + data_json_after_str
    file_out = open('data_after.js', 'w')
    file_out.write(data_json_after_str)

set_data_json_after(p_list_after)

# data_json_after = set_data_json_after(p_list_after)

plt.plot(get_perferance_point_list('data.js'))
plt.plot(get_perferance_point_list('data_after.js'))
plt.show()
# for each benchmark

pass

