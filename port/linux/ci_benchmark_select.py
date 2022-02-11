#!/usr/bin/python3
import json
from select import select
import sys
import os
import shutil

with open('performance_data.json', 'r') as perf_json_file:
    perf_json_data: list = json.load(perf_json_file)
    perf_data_with_index = []
    for i in range(perf_json_data.__len__()):
        perf_data_with_index.append({'index': i+1, 'value': perf_json_data[i]})

    perf_data_with_index_sorted = sorted(
        perf_data_with_index, key=lambda e: e['value'])

    selected = perf_data_with_index_sorted[int(perf_json_data.__len__()/2)]
    print('=============================================')
    print('selected: ', selected)

    shutil.copy('benchmark_result_' +
                str(selected['index']) + '.json', 'benchmark_result.json')
