import modbus

mb = modbus.ModBusRTU(128, 128)
mb.setSlave(1)

send_buff = mb.serializeReadRegisters(0, 10)
print(send_buff)

host_regists = mb.deserializeReadRegisters(
    b'\x01\x03\x14\x00\x00\x00\x00\x04\xD2\x00\x00\x00\x00\x00\x7B\x00\x00\x00\x00\x00\x00\x00\x00\xE5\x0B'
)
print(host_regists)


mb.serializeReadInputRegisters(0, 2)

mb.deserializeReadInputRegisters(b'\x01\x04\x04\x00\x00\x08\xE6\x7D\xCE')


send_buff = mb.serializeWriteRegister(0, 0x1234)
print(send_buff)

send_buff = mb.serializeWriteBits(0, [1, 1, 1, 0, 1, 0, 1, 0])
print(send_buff)
