
class Test:
    def test_default(self,
                     dev: str,
                     baudrate=115200,
                     bytesize=8,
                     parity='N',
                     stopbits=1,
                     xonxoff=0
                     ):
        return dev, baudrate, bytesize, parity, stopbits, xonxoff


def test_default(
    dev: str,
    baudrate=115200,
    bytesize=8,
    parity='N',
    stopbits=1,
    xonxoff=0
):
    return dev, baudrate, bytesize, parity, stopbits, xonxoff


dev, baudrate, bytesize, parity, stopbits, xonxoff = test_default(
    'uart3', 9600)

assert dev == 'uart3'
assert baudrate == 9600
assert bytesize == 8
assert parity == 'N'
assert stopbits == 1
assert xonxoff == 0

test = Test()
dev, baudrate, bytesize, parity, stopbits, xonxoff = test.test_default(
    'uart3', 9600)

assert dev == 'uart3'
assert baudrate == 9600
assert bytesize == 8
assert parity == 'N'
assert stopbits == 1
assert xonxoff == 0
print('PASS')
