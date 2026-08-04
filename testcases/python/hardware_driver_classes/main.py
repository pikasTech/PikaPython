from machine import I2C, SPI, UART


class RegisterDevice:
    def __init__(self, identifier, address):
        self.bus = I2C(identifier, freq=400000)
        self.address = address

    def read_register(self, register):
        bus = self.bus
        address = self.address
        data = bytearray(b"\x00\x00")
        bus.readfrom_into(address + register, data)
        return data[0] + data[1]


class SpiDevice:
    def __init__(self, identifier):
        self.bus = SPI(identifier, baudrate=2000000)

    def exchange(self):
        bus = self.bus
        source = b"AZ"
        destination = bytearray(b"\x00\x00")
        bus.write_readinto(source, destination)
        return destination[0] + destination[1]


class UartParser:
    def __init__(self, identifier):
        self.uart = UART(identifier, baudrate=115200)

    def checksum(self):
        uart = self.uart
        frame = bytearray(b"\x00\x00\x00\x00")
        count = uart.readinto(frame, nbytes=3)
        total = 0
        for index in range(count):
            total += frame[index]
        return total


register_device = RegisterDevice(0, 0x40)
spi_device = SpiDevice(2)
uart_parser = UartParser(1)

print(register_device.read_register(2))
print(spi_device.exchange())
print(uart_parser.checksum())
