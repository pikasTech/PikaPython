import json
data = '{"code": 0,"path": "act_mode_config", "act_mode": [["ACTUATOR_NAME","ACTUATOR_ID","ACTUATOR_MODE_NAME","ACTUATOR_MODE_ID","ACT_MODE_TYPE","ACT_VALUE_TIME_s"],["电机1","actuator123","开闭循环1","mode2345","loop","{\\"AC_VALUE\\":1,\\"ACT_TIME_s\\":300}"],["电机1","actuator123","常开","mode2346","always","{\\"AC_VALUE\\":1,\\"ACT_TIME_s\\"}:0"]],"ext": "data"}'

print(data)
json_data = json.loads(data)
ACT_VALUE_TIMS_S = json_data['act_mode'][1][5]
print(ACT_VALUE_TIMS_S)
act_value_tims_json = json.loads(ACT_VALUE_TIMS_S)
print(act_value_tims_json['ACT_TIME_s'])
act_time_s = act_value_tims_json['ACT_TIME_s']
