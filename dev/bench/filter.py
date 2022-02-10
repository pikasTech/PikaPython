#! /usr/bin/python3
import json
import matplotlib.pyplot as plt
import numpy as np

def get_benchmark_list()->list:
    data_file = open('data.js','r')
    data_js_prefix = "window.BENCHMARK_DATA = "
    data_json_str = data_file.read().replace(data_js_prefix, "")
    data_json = json.loads(data_json_str)
    return data_json['entries']['Benchmark']


def get_perferance_point_list()->list:
    benchmark_list = get_benchmark_list()
    perferance_point_list:list = []
    for i in range(5, benchmark_list.__len__() - 5):
        benchmark = benchmark_list[i]
        benches:list = benchmark['benches']
        # for j in range(0, benches.__len__()):
        perferance_point_list.append(benches[0]['value'])
    return perferance_point_list

p_list = get_perferance_point_list()

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

plt.plot(p_list)
plt.plot(p_list_after)
plt.show()
# for each benchmark

pass

