import modbus

# Create a ModBusRTU object, specify the send buffer and receive buffer size as 128 bytes
mb = modbus.ModBusRTU(128, 128)

# Set slave address to 1
op_code = 'x06'
slave_addr = 0x13
reg_addr = 0x00
reg_value = 0x01
mb.serializeRequest(op_code, reg_addr, reg_value, slave_addr)
res = b'\x13\x06\x00\x00\x00\x01\x4b\x78'
ret = mb.deserializeResponse('x06', res)
assert ret == 1
