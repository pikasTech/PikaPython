import PikaStdDevice

class GPIO(PikaStdDevice.GPIO):
    def platformGetEventId(self):
        return "gpio"

    def __init__(self, pin: str = None, mode: str = None):
        super().__init__()
        if pin:
            self.setPin(pin)
        if mode:
            self.setMode(mode)
        self.enable()

io = GPIO("PA1", "out")
io.close()
