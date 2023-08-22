def convert_to_modbus(num):
    # 判断输入数值的符号
    if num >= 0:
        sign = 0
    else:
        sign = 1

    # 如果是负数，将其转换为补码形式
    if sign:
        num = (~(-num) + 1) & 0xFFFF

    return num


def convert_from_modbus(num):
    # 判断输入数值的符号
    if (num & 0x8000):
        sign = 1
    else:
        sign = 0

    # 如果是负数，将其转换回原始的负数形式
    if sign:
        num = -((~num + 1) & 0xFFFF)

    return num


# 测试示例
num = -10
modbus_value = convert_to_modbus(num)

original_value = convert_from_modbus(modbus_value)

assert modbus_value == 65526
assert original_value == -10
print("PASS")
