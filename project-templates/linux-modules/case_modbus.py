import modbus

request = modbus.ModBusRTU(1).serializeReadRegisters(16, 2)
tcp = modbus.ModBusTCP(1, 7).serializeReadRegisters(16, 2)
print("modbus-crc:", modbus.check_crc(request))
print("modbus-rtu-bytes:", len(request))
print("modbus-tcp-header:", tcp[0], tcp[1], tcp[4], tcp[5])
print("modbus-registers:", modbus.decode_registers(b"\x00\x01\x12\x34"))
