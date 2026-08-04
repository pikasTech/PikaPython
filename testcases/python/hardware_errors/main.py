from machine import SPI

spi = SPI(2, baudrate=2000000)
destination = bytearray(b"\x00")

try:
    spi.write_readinto(b"AZ", destination)
except ValueError:
    print(7)
finally:
    print(destination[0])

print(9)
