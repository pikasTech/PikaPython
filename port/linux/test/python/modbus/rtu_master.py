# Import modbus module
import modbus

# Create a ModBusRTU object, specify the send buffer and receive buffer size as 128 bytes
mb = modbus.ModBusRTU(128, 128)

# Set slave address to 1
mb.setSlave(1)

# Generate a request frame for reading registers, specify the start address as 0 and the quantity as 10
send_buff = mb.serializeReadRegisters(0, 10)

# Print the byte string of the request frame
print(send_buff)

# Parse a response frame for reading registers, return a list containing the values of the registers
host_regists = mb.deserializeReadRegisters(
    b'\x01\x03\x14\x00\x00\x00\x00\x04\xD2\x00\x00\x00\x00\x00\x7B\x00\x00\x00\x00\x00\x00\x00\x00\xE5\x0B'
)
print(host_regists)


# Generate a request frame for reading input registers, specify the start address as 0 and the quantity as 2
mb.serializeReadInputRegisters(0, 2)

# Parse a response frame for reading input registers, return a list containing the values of the input registers

mb.deserializeReadInputRegisters(b'\x01\x04\x04\x00\x00\x08\xE6\x7D\xCE')


# Generate a request frame for writing a single register, specify the register address as 0 and the value as 0x1234
send_buff = mb.serializeWriteRegister(0, 0x1234)
print(send_buff)

send_buff = mb.serializeWriteBits(0, [1, 1, 1, 0, 1, 0, 1, 0])
print(send_buff)
