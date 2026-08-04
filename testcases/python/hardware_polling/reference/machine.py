IDLE = 0


class Pin:
    IN = 0
    OUT = 1
    OPEN_DRAIN = 2
    PULL_UP = 1
    PULL_DOWN = 2

    def __init__(self, identifier, mode=-1, pull=-1, value=None):
        if identifier < 0 or identifier > 255:
            raise ValueError("invalid pin identifier")
        if mode == -1:
            mode = self.IN
        if pull == -1:
            pull = 0
        self._validate_config(mode, pull, value)
        self.identifier = identifier
        self.mode = mode
        self.pull = pull
        self.level = 0 if value is None else int(value)
        self.handler = None

    @classmethod
    def _validate_config(cls, mode, pull, value):
        if mode not in (cls.IN, cls.OUT, cls.OPEN_DRAIN):
            raise ValueError("invalid pin mode")
        if pull not in (0, cls.PULL_UP, cls.PULL_DOWN):
            raise ValueError("invalid pin pull")
        if mode == cls.OUT and pull != 0:
            raise ValueError("output pin does not accept pull")
        if mode == cls.OPEN_DRAIN and pull == cls.PULL_DOWN:
            raise ValueError("open-drain pin does not accept pull-down")
        if value is not None and value not in (0, 1, False, True):
            raise ValueError("invalid pin value")
        if mode == cls.IN and value is not None:
            raise ValueError("input pin does not accept initial value")

    def init(self, mode=-1, pull=-1, value=None):
        next_mode = self.mode if mode == -1 else mode
        next_pull = self.pull if pull == -1 else pull
        self._validate_config(next_mode, next_pull, value)
        self.mode = next_mode
        self.pull = next_pull
        if value is not None:
            self.level = int(value)

    def value(self, level=None):
        if level is None:
            return self.level
        if self.mode == self.IN:
            raise ValueError("input pin is not writable")
        if level not in (0, 1, False, True):
            raise ValueError("invalid pin value")
        self.level = int(level)

    def on(self):
        self.value(1)

    def off(self):
        self.value(0)

    def irq(self, handler):
        self.handler = handler

    def trigger(self):
        self.handler(self)


class ADC:
    def __init__(self, identifier):
        identifier = (
            identifier.identifier
            if isinstance(identifier, Pin)
            else identifier
        )
        if not isinstance(identifier, int):
            raise TypeError("ADC source must be an integer or Pin")
        if identifier < 0 or identifier > 15:
            raise ValueError("invalid ADC source")
        self.identifier = identifier

    def read_u16(self):
        if self.identifier == 0:
            return 0
        if self.identifier == 1:
            return 32768
        if self.identifier == 2:
            return 65535
        return self.identifier * 4096 + 123

    def read_uv(self):
        raise RuntimeError("ADC voltage calibration unavailable")


class PWM:
    def __init__(self, pin, freq=1000):
        self.pin = pin
        self.frequency = freq
        self.duty = 0

    def freq(self, value=-1):
        if value != -1:
            self.frequency = value
        return self.frequency

    def duty_u16(self, value=-1):
        if value != -1:
            self.duty = value
        return self.duty


class WDT:
    def __init__(self, timeout=5000):
        self.timeout = timeout
        self.feed_count = 0

    def feed(self):
        self.feed_count += 1


class UART:
    def __init__(self, identifier, baudrate=9600):
        self.identifier = identifier
        self.baudrate = baudrate
        self.handler = None

    def readinto(self, buffer, nbytes=-1):
        count = len(buffer) if nbytes == -1 else min(len(buffer), nbytes)
        for index in range(count):
            buffer[index] = 64 + self.identifier + index
        return count

    def irq(self, handler):
        self.handler = handler

    def trigger(self):
        self.handler(self)


class I2C:
    def __init__(self, identifier, freq=400000):
        self.identifier = identifier
        self.freq = freq

    def scan(self):
        return [24, 64]

    def readfrom_into(self, address, buffer):
        for index in range(len(buffer)):
            buffer[index] = address + index


class SPI:
    def __init__(self, identifier, baudrate=1000000):
        self.identifier = identifier
        self.baudrate = baudrate

    def write_readinto(self, source, destination):
        if len(source) != len(destination):
            raise ValueError("buffers must have equal length")
        for index in range(len(source)):
            destination[index] = source[index]


class Timer:
    PERIODIC = 1

    def __init__(self, identifier=-1):
        self.identifier = identifier
        self.handler = None

    def init(self, callback):
        self.handler = callback

    def trigger(self):
        self.handler(self)
