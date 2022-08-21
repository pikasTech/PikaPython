import TemplateDevice

def EXPECT_EQ(test_name, input, expected):
    print('-----TEST-----')
    print(test_name)
    if input != expected:
        print('[Error]')
        print(test_name)
        print('[info] Expected:')
        print(expected)
        print('[info] Input:')
        print(input)
        # error
        while True:
            is_error = 1
    else:
        print('[ OK  ]')

def test():
    uart = TemplateDevice.UART()
    uart.setId(1)
    uart.setBaudRate(115200)
    bt = uart.readBytes(5)
    EXPECT_EQ('uart.readBytes', bt, b'\x00\x01\x02\x03\x04')
    uart.writeBytes(b'\xff\xfe\xfd\xfc', 4)

