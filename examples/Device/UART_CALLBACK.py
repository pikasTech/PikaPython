import PikaStdDevice as std
print('hello pikascript')

uart = std.UART()
uart.setId(0)
uart.setBaudRate(115200)
uart.enable()

def cb1(signal):
    print('recv:', uart.read(32))

uart.setCallBack(cb1, uart.SIGNAL_RX)

while True:
    pass
