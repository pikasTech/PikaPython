import json
import time

dict_test = {'widget0': {"type": "div", "attributes": {"class": "container", "style": {"width": 240, "height": 240, "margin": 0, "padding": 0, "border-radius": 0, "border-width": 0, "border-color": "red", "background-color": 37864}}, "nodes": [{"type": "text", "attributes": {"font-size": "30", "class": "result-text", "style": {"top": 5, "left": 5, "width": 220, "text-align": "right", "height": 30, "color": "white", "text-overflow": "longbreak", "border-width": 0, "border-color": "red", "background-color": "transparent"}}, "nodes": [], "bindings":[{"attrName": "text", "key": "result", "isText": True}], "events": [], "value":"0", "widgetName":"widget1"}, {"type": "div", "attributes": {"class": "key-wrapper", "onclick": "onclick", "style": {"width": 60, "height": 50, "border-width": 0, "border-color": "red", "background-color": "transparent", "top": 40, "left": 10}}, "nodes": [{"type": "text", "attributes": {"font-size": "30", "class": "key", "style": {"left": 0, "top": 0, "width": 50, "height": 40, "margin": 0, "padding": 0, "color": "white", "font-size": 30, "text-align": "center", "background-color": "transparent"}}, "nodes": [], "bindings":[], "events":[], "value":"1", "widgetName":"widget3"}], "bindings":[], "events":[{"onclick": "onclick"}], "widgetName": "widget2"}, {"type": "div", "attributes": {"class": "key-wrapper", "onclick": "onclick", "style": {"width": 60, "height": 50, "border-width": 0, "border-color": "red", "background-color": "transparent", "top": 40, "left": 60}}, "nodes": [{"type": "text", "attributes": {"font-size": "30", "class": "key", "style": {
    "left": 0, "top": 0, "width": 50, "height": 40, "margin": 0, "padding": 0, "color": "white", "font-size": 30, "text-align": "center", "background-color": "transparent"}}, "nodes": [], "bindings":[], "events":[], "value":"2", "widgetName":"widget5"}], "bindings":[], "events":[{"onclick": "onclick"}], "widgetName": "widget4"}, {"type": "div", "attributes": {"class": "key-wrapper", "onclick": "onclick", "style": {"width": 60, "height": 50, "border-width": 0, "border-color": "red", "background-color": "transparent", "top": 40, "left": 110}}, "nodes": [{"type": "text", "attributes": {"font-size": "30", "class": "key", "style": {"left": 0, "top": 0, "width": 50, "height": 40, "margin": 0, "padding": 0, "color": "white", "font-size": 30, "text-align": "center", "background-color": "transparent"}}, "nodes": [], "bindings":[], "events":[], "value":"3", "widgetName":"widget7"}], "bindings":[], "events":[{"onclick": "onclick"}], "widgetName": "widget6"}, {"type": "div", "attributes": {"class": "key-wrapper", "onclick": "onclick", "style": {"width": 60, "height": 50, "border-width": 0, "border-color": "red", "background-color": "transparent", "top": 40, "left": 160}}, "nodes": [{"type": "text", "attributes": {"font-size": "30", "class": "key", "style": {"left": 0, "top": 0, "width": 50, "height": 40, "margin": 0, "padding": 0, "color": "white", "font-size": 30, "text-align": "center", "background-color": "transparent"}}, "nodes": [], "bindings":[], "events":[], "value":"+", "widgetName":"widget9"}], "bindings":[], "events":[{"onclick": "onclick"}], "widgetName": "widget8"}], "bindings": [], "events": [], "widgetName": "widget0"}}

json.CONFIG_USING_CJSON = True
print('dict_test:', dict_test)
json_test = json.dumps(dict_test)
print('json_test:', json_test)

json.CONFIG_USING_CJSON = True
start = time.tick_ms()
for i in range(10):
    res = json.loads(json_test)
str_res_cjson = str(res)
print('len(str_res_cjson):', len(str_res_cjson))
end = time.tick_ms()
time_cjson_loads = end - start
print('loads: cjson:', time_cjson_loads, 'ms')

json.CONFIG_USING_CJSON = False
start = time.tick_ms()
for i in range(10):
    res = json.loads(json_test)
str_res_jsmn = str(res)
print('len(str_res_jsmn):', len(str_res_jsmn))
end = time.tick_ms()
time_jsmn_loads = end - start

print('loads: jsmn:', time_jsmn_loads, 'ms')

# test for dumps

# test for dumps

json.CONFIG_USING_CJSON = True
start = time.tick_ms()
for i in range(10):
    res = json.dumps(dict_test)
len_res_cjson = len(res)
print('len(res_cjson):', len_res_cjson)
end = time.tick_ms()
time_cjson_dumps = end - start
print('dumps: cjson:', time_cjson_dumps, 'ms')

json.CONFIG_USING_CJSON = False
start = time.tick_ms()
for i in range(10):
    res = json.dumps(dict_test)
len_res_jsmn = len(res)
print('len(res_jsmn):', len_res_jsmn)
end = time.tick_ms()
time_jsmn_dumps = end - start
print('dumps: jsmn:', time_jsmn_dumps, 'ms')

print('==================================================')
print('loads: jsmn is', (time_cjson_loads /
      time_jsmn_loads), 'times faster than cjson')
print('dumps: jsmn is', (time_cjson_dumps /
      time_jsmn_dumps), 'times faster than cjson')
