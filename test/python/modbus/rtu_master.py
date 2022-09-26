import modbus

mb_tcp = modbus.ModBusRTU(128, 128)
mb_tcp.setSlave(1)

send_buff = mb_tcp.serializeReadRegisters(0, 10)
print(send_buff)

host_regists = mb_tcp.deserializeReadRegisters(
    b'\x01\x03\x14\x00\x00\x00\x00\x04\xD2\x00\x00\x00\x00\x00\x7B\x00\x00\x00\x00\x00\x00\x00\x00\xE5\x0B'
)
print(host_regists)


send_buff = mb_tcp.serializeWriteRegister(0, 0x1234)
print(send_buff)
